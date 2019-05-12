#include <stddef.h>
#include <locale.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <signal.h>
#include <wait.h>
#include <time.h>

#include "../common/fdb.h"
#include "../common/commands.h"
#include "../common/artigo.h"
#include "../common/stock.h"
#include "../common/strings.h"
#include "../common/sv_protocol.h"
#include "../common/util.h"
#include "../common/venda.h"

#include "sv.h"

void server_shutdown();

fdb_t fdbStdin, fdbStdout, fdbStderr;

void server_startup() {
    // Inicializar file descriptor buffers para o stdin e stdout e stderr, pouco provável de falhar
    fdb_create(&fdbStdin, STDIN_FILENO);
    fdb_create(&fdbStdout, STDOUT_FILENO);
    fdb_create(&fdbStderr, STDERR_FILENO);

    // Signal handlers for terminating gracefully
    signal(SIGTERM, server_shutdown);
    signal(SIGINT, server_shutdown);

    // Inicializar os ficheiros necessários
    setlocale(LC_ALL, "Portuguese");

    inicializar_ficheiro_strings();
    inicializar_ficheiro_artigos();
    file_open(&g_pFdbStocks, NOME_FICHEIRO_STOCKS, true, true);
    file_open(&g_pFdbVendas, NOME_FICHEIRO_VENDAS, true, true);

    // Criar e abrir a fifo necessária à comunicação
    if(fdb_mkfifo(&g_pFdbServerFifo, SV_FIFO_NAME, O_RDONLY, 0644) != 0) {
        fdb_printf(fdbStdout, "Não foi possível criar uma FIFO para o servidor receber instruções! Terminando...\n");
        server_shutdown();
    }
}

bool exec_mostrar_stock_e_preco(pid_t requesterPid, fdb_t fdbFifoResposta) {// Esta instrução lê do pipe os seguintes argumentos:
    // long => codigo
    // pid_t => pid do processo que requereu os dados
    // Esta instrução escreve no pipe de resposta os seguintes argumentos:
    // bool => estado de sucesso da instrução
    // long => quantidade
    // double => preço

    long codigo;
    if(fdb_read(g_pFdbServerFifo, &codigo, sizeof(codigo)) < 0) {
        fdb_printf(fdbStderr, "[MOSTRAR_STOCK_E_PRECO] Erro ao ler o código do artigo do FIFO! Terminando...\n");
        return false;
    }

    long quantidade;
    double preco;
    if(mostra_info_artigo(codigo, &quantidade, &preco) != 0) {
        fdb_printf(fdbStderr, "[MOSTRAR_STOCK_E_PRECO] Não foi possível obter as informações do artigo %ld!\n", codigo);
        return false;
    }

    // Escrever a resposta para o FIFO de resposta

    static const bool success = true;
    fdb_write(fdbFifoResposta, &success, sizeof(success));

    fdb_write(fdbFifoResposta, &quantidade, sizeof(quantidade));
    fdb_write(fdbFifoResposta, &preco, sizeof(preco));

    printf("[MOSTRAR_STOCK_E_PRECO] [LOG] [%d] Codigo=%ld; Quantidade=%ld, Preço=%f\n", requesterPid, codigo, quantidade, preco);

    return true;
}

bool exec_atualizar_stock_mostrar_novo_stock(pid_t requesterPid, fdb_t fdbFifoResposta) {// Esta instrução lê do pipe os seguintes argumentos:
    // long => codigo
    // long => quantidade
    // pid_t => pid do processo que requereu os dados
    // Esta instrução escreve no pipe de resposta os seguintes argumentos:
    // bool => estado de sucesso da instrução
    // long => novoStock

    long codigo;
    if(fdb_read(g_pFdbServerFifo, &codigo, sizeof(codigo)) < 0) {
        fdb_printf(fdbStderr, "[ATUALIZAR_STOCK_E_MOSTRAR_NOVO_STOCK] Erro ao ler o código do artigo do FIFO! Terminando...\n");
        return false;
    }

    long acrescento;
    if(fdb_read(g_pFdbServerFifo, &acrescento, sizeof(acrescento)) < 0) {
        fdb_printf(fdbStderr, "[ATUALIZAR_STOCK_E_MOSTRAR_NOVO_STOCK] Erro ao ler o acrescento do FIFO! Terminando...\n");
        return false;
    }

    long novoStock;
    if(atualiza_mostra_stock(codigo, acrescento, &novoStock) != 0) {
        fdb_printf(fdbStderr, "[ATUALIZAR_STOCK_E_MOSTRAR_NOVO_STOCK] Não foi possível atualizar o stock do artigo! Terminando...\n");
        return false;
    }

    // Escrever a resposta para o FIFO de resposta
    static const bool success = true;
    fdb_write(fdbFifoResposta, &success, sizeof(success));

    fdb_write(fdbFifoResposta, &novoStock, sizeof(novoStock));

    printf("[ATUALIZAR_STOCK_E_MOSTRAR_NOVO_STOCK] [LOG] [%d] Codigo=%ld, Quantidade=%ld; NovoStock=%ld\n", requesterPid, codigo, acrescento, novoStock);

    return true;
}

void exec_ag(pid_t requesterPid) {
    // Criar uma pipe para onde enviar a input do ag
    int inputPipe[2];
    pipe(inputPipe);

    // Antes de executar o AG, precisamos de saber quando se deu a última agregação
    // O offset onde se deu a última agregação está guardado na posição 0 do ficheiro de vendas
    if(fdb_lseek(g_pFdbVendas, 0, SEEK_SET) != 0) {
        fdb_printf(fdbStderr, "[AG] [LOG] [%d] Erro ao obter o offset da última agregação!\n", requesterPid);
        return;
    }

    ssize_t offsetUltimaAgregacao;
    if(fdb_read(g_pFdbVendas, &offsetUltimaAgregacao, sizeof(offsetUltimaAgregacao)) <= 0) {
        fdb_printf(fdbStderr, "[AG] [LOG] [%d] Erro ao ler o offset da última agregação!\n", requesterPid);
        return;
    }

    // Fazer lseek para a posição da última agregação
    if(fdb_lseek(g_pFdbVendas, offsetUltimaAgregacao, SEEK_SET) != offsetUltimaAgregacao) {
        fdb_printf(fdbStderr, "[AG] [LOG] Não foi possível fazer lseek para o offset da última agregação! (Será que já foram feitas vendas?)\n");
        return;
    }

    // Executar o AG num processo separado
    pid_t childProcess = fork();

    if(childProcess == 0) {
        // Fechar o lado de escrita
        close(inputPipe[1]);

        // Obter a data e hora atuais
        time_t rawtime;
        time(&rawtime);

        // Converter para string
        char outputFileName[1024];
        strftime(outputFileName, sizeof(outputFileName), "%FT%T.txt", localtime(&rawtime));

        // O AG deve executar num processo separado, com o stdin redirecionado e o stdout também

        // Abrir um file descriptor para o ficheiro agregado de vendas
        fdb_t fdbOutputFile;
        if(file_open(&fdbOutputFile, outputFileName, false, true) != 0) {
            fdb_printf(fdbStderr, "[AG] [LOG] [%d] Erro ao criar o ficheiro de output!\n", requesterPid);
            _exit(-1);
        }

        // Preparar o ambiente do AG
        if(dup2(inputPipe[0], STDIN_FILENO) < 0) {
            perror("dup2(..., STDIN_FILENO) failed");
            fdb_printf(fdbStderr, "[AG] [LOG] dup2(..., STDIN_FILENO) falhou!\n");
            _exit(-2);
        }

        if(dup2(fdbOutputFile->fd, STDOUT_FILENO) < 0) {
            perror("dup2(..., STDOUT_FILENO) failed");
            fdb_printf(fdbStderr, "[AG] [LOG] dup2(..., STDOUT_FILENO) falhou!\n");
            _exit(-3);
        }

        // Executar o AG
        if(execl("./ag", "./ag", NULL) != 0)
            perror("execl() failed");

        // O filho nunca executa para além daqui!
        _exit(-4);
    } else {
        // Fechar o lado de leitura da pipe
        close(inputPipe[0]);

        // Continuar as preparações do ambiente do filho
        fdb_t fdbInputPipe;
        fdb_create(&fdbInputPipe, inputPipe[1]);

        // Copiar tudo o que ainda não foi agregado para a pipe
        ssize_t bytesRead;
        char buf[4096];
        while((bytesRead = fdb_read(g_pFdbVendas, buf, sizeof(buf))) > 0) {
            fdb_write(fdbInputPipe, buf, bytesRead);
            fdb_write(fdbStdout, buf, bytesRead);
        }
        fprintf(stderr, "Last bytesRead=%ld\n", bytesRead);

        // Fechar o lado de escrita
        fdb_fclose(fdbInputPipe);

        // Esperar que o filho termine
        int status;
        waitpid(childProcess, &status, 0);

        if(!WIFEXITED(status)) {
            // Erro ao executar o AG!
            fdb_printf(fdbStderr, "[EXECUTAR_AG] [LOG] [%d] Erro ao executar o AG! Status code: %d, Return code: %d\n", requesterPid, status, WEXITSTATUS(status));
            return;
        }

        // TODO: Modificar a posição da última agregação
    }
}

int main() {
    server_startup();

    // Este servidor funciona com instruções:
    // Lê a partir do fifo um código de instrução (uma char)
    // Interpreta a instrução de acordo com as instruções que é capaz de aceitar
    // Depois, no código responsável por processar cada instrução, lê a partir do pipe os argumentos respetivos dessa instrução,
    // que se encontram descritos abaixo
    instruction_t instruction;
    while(fdb_read(g_pFdbServerFifo, &instruction, sizeof(instruction)) == sizeof(instruction)) {
        // Obter o PID do processo a partir da pipe
        pid_t requesterPid;
        if(fdb_read(g_pFdbServerFifo, &requesterPid, sizeof(requesterPid)) < 0) {
            fdb_printf(fdbStderr, "Erro ao ler o PID do FIFO! Terminando...\n");
            break;
        }

        if(instruction == SV_INSTRUCTION_EXECUTAR_AG) {
            // A instrução de executar o AG tem código especial, pois esta não aceita FIFO de resposta
            exec_ag(requesterPid);
            continue;
        }

        char requesterPidStr[128];
        calcularFifoResposta(requesterPid, requesterPidStr);

        // Usando o ID do processo, podemos agora abrir a pipe de output deste processo,
        // e escrever o resultado da operação para essa pipe
        fdb_t fdbFifoResposta;
        file_open(&fdbFifoResposta, requesterPidStr, false, true);

        bool success = false;

        if(instruction == SV_INSTRUCTION_MOSTRAR_STOCK_E_PRECO) {
            success = exec_mostrar_stock_e_preco(requesterPid, fdbFifoResposta);
        } else if(instruction == SV_INSTRUCTION_ATUALIZAR_STOCK_E_MOSTRAR_NOVO_STOCK) {
            success = exec_atualizar_stock_mostrar_novo_stock(requesterPid, fdbFifoResposta);
        }

        if(!success) {
            fdb_write(fdbFifoResposta, &success, sizeof(success));
            fdb_printf(fdbStderr, "Erro ao processar uma instrução %d!\n", instruction);
        }

        // Fechar o FIFO de resposta
        file_close(fdbFifoResposta);
    }

    server_shutdown();

    // Sucesso
    return 0;
}

void server_shutdown() {
    // Fechar os ficheiros abertos
    file_close(g_pFdbVendas);
    file_close(g_pFdbStocks);
    file_close(g_pFdbArtigos);
    file_close(g_pFdbStrings);

    // Fechar a fifo de comunicação
    fdb_unlink(g_pFdbServerFifo);

    // Fechar os fdb_ts do stdin, stdout e stderr
    fdb_destroy(fdbStderr);
    fdb_destroy(fdbStdout);
    fdb_destroy(fdbStdin);

    _exit(0);
}
