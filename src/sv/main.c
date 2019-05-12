#include <stddef.h>
#include <locale.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <signal.h>

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

        char requesterPidStr[128];
        calcularFifoResposta(requesterPid, requesterPidStr);

        // Usando o ID do processo, podemos agora abrir a pipe de output deste processo,
        // e escrever o resultado da operação para essa pipe
        fdb_t fdbFifoResposta;
        file_open(&fdbFifoResposta, requesterPidStr, false, true);

        bool success = false;

        if(instruction == SV_INSTRUCTION_MOSTRAR_STOCK_E_PRECO) {
            // Esta instrução lê do pipe os seguintes argumentos:
            // long => codigo
            // pid_t => pid do processo que requereu os dados
            // Esta instrução escreve no pipe de resposta os seguintes argumentos:
            // long => quantidade
            // double => preço

            long codigo;
            if(fdb_read(g_pFdbServerFifo, &codigo, sizeof(codigo)) < 0) {
                fdb_printf(fdbStderr, "[MOSTRAR_STOCK_E_PRECO] Erro ao ler o código do artigo do FIFO! Terminando...\n");
                break;
            }

            long quantidade;
            double preco;
            if(mostra_info_artigo(codigo, &quantidade, &preco) != 0) {
                fdb_printf(fdbStderr, "[MOSTRAR_STOCK_E_PRECO] Não foi possível obter as informações do artigo %ld!\n", codigo);
            } else {
                // Escrever a resposta para o FIFO de resposta

                success = true;
                fdb_write(fdbFifoResposta, &success, sizeof(success));

                fdb_write(fdbFifoResposta, &quantidade, sizeof(quantidade));
                fdb_write(fdbFifoResposta, &preco, sizeof(preco));

                printf("[MOSTRAR_STOCK_E_PRECO] [LOG] [%d] Codigo=%ld; Quantidade=%ld, Preço=%f\n", requesterPid, codigo, quantidade, preco);
            }
        } else if(instruction == SV_INSTRUCTION_ATUALIZAR_STOCK_E_MOSTRAR_NOVO_STOCK) {
            // Esta instrução lê do pipe os seguintes argumentos:
            // long => codigo
            // long => quantidade
            // pid_t => pid do processo que requereu os dados
            // Esta instrução escreve no pipe de resposta os seguintes argumentos:
            // long => novoStock

            long codigo;
            if(fdb_read(g_pFdbServerFifo, &codigo, sizeof(codigo)) < 0) {
                fdb_printf(fdbStderr, "[ATUALIZAR_STOCK_E_MOSTRAR_NOVO_STOCK] Erro ao ler o código do artigo do FIFO! Terminando...\n");
                break;
            }

            long acrescento;
            if(fdb_read(g_pFdbServerFifo, &acrescento, sizeof(acrescento)) < 0) {
                fdb_printf(fdbStderr, "[ATUALIZAR_STOCK_E_MOSTRAR_NOVO_STOCK] Erro ao ler o acrescento do FIFO! Terminando...\n");
                break;
            }

            long novoStock;
            if(atualiza_mostra_stock(codigo, acrescento, &novoStock) != 0) {
                fdb_printf(fdbStderr, "[ATUALIZAR_STOCK_E_MOSTRAR_NOVO_STOCK] Não foi possível atualizar o stock do artigo! Terminando...\n");
            } else {
                // Escrever a resposta para o FIFO de resposta
                success = true;
                fdb_write(fdbFifoResposta, &success, sizeof(success));
                fdb_write(fdbFifoResposta, &novoStock, sizeof(novoStock));

                printf("[ATUALIZAR_STOCK_E_MOSTRAR_NOVO_STOCK] [LOG] [%d] Codigo=%ld, Quantidade=%ld; NovoStock=%ld\n", requesterPid, codigo, acrescento, novoStock);
            }
        } else if(instruction == SV_INSTRUCTION_EXECUTAR_AG) {
            // TODO: Mandar executar o AG!
            // O AG deve executar num processo separado, com o stdin redirecionado e o stdout também

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
    file_close(g_pFdbServerFifo);
    unlink(SV_FIFO_NAME);

    // Fechar os fdb_ts do stdin, stdout e stderr
    fdb_destroy(fdbStderr);
    fdb_destroy(fdbStdout);
    fdb_destroy(fdbStdin);

    _exit(0);
}
