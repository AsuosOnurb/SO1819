#include <stddef.h>
#include <locale.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#include "../common/fdb.h"
#include "../common/commands.h"
#include "../common/artigo.h"
#include "../common/stock.h"
#include "../common/strings.h"
#include "../common/sv_backend.h"
#include "../common/util.h"
#include "../common/venda.h"

#include "sv.h"

int main(int argc, const char *argv[]) {
    // Verificar os parâmetros do programa
    if (argc > 1) {
        printf("Este programa não utiliza argumentos na sua invocação.\n");
        return -1;
    }

    // Inicializar file descriptor buffers para o stdin e stdout
    fdb_t fdbStdin, fdbStdout, fdbStderr;
    fdb_create(STDIN_FILENO, &fdbStdin);
    fdb_create(STDOUT_FILENO, &fdbStdout);
    fdb_create(STDERR_FILENO, &fdbStderr);

    // Inicializar os ficheiros necessários
    setlocale(LC_ALL, "Portuguese");

    file_open(&g_pFdbStrings, NOME_FICHEIRO_STRINGS, true, true);
    inicializar_ficheiro_artigos();
    file_open(&g_pFdbStocks, NOME_FICHEIRO_STOCKS, true, true);
    file_open(&g_pFdbVendas, NOME_FICHEIRO_VENDAS, true, true);

    // Criar e abrir a fifo necessária à comunicação
    int fifoFd;
    if((fifoFd = mkfifo(SV_FIFO_NAME, 0644)) < 0) {
        fdb_printf(fdbStdout, "Não foi possível criar uma FIFO para o servidor receber instruções! Errcode: %ld\n", fifoFd);
        return -2;
    }

    fdb_t fdbFifo;
    fdb_create(STDIN_FILENO, &fdbFifo); // Unlikely to fail

    // Este servidor funciona com instruções:
    // Lê a partir do fifo um código de instrução (uma char)
    // Interpreta a instrução de acordo com as instruções que é capaz de aceitar
    // Depois, no código responsável por processar cada instrução, lê a partir do pipe os argumentos respetivos dessa instrução,
    // que se encontram descritos abaixo
    instruction_t instruction;
    while(fdb_read(fdbFifo, &instruction, 1) == 1) {
        char *result = NULL;
        ssize_t dataSize = 0;

        if(instruction == SV_INSTRUCTION_MOSTRAR_STOCK_E_PRECO) {
            // Esta instrução lê do pipe os seguintes argumentos:
            // long => codigo
            // pid_t => pid do processo que requereu os dados
            // Esta instrução escreve no pipe de resposta os seguintes argumentos:
            // long => quantidade
            // double => preço

            long codigo;
            if(fdb_read(fdbFifo, &codigo, sizeof(codigo)) < 0) {
                fdb_printf(fdbStderr, "[MOSTRAR_STOCK_E_PRECO] Erro ao ler o código do artigo do FIFO! Terminando...\n");
                break;
            }

            long quantidade;
            double preco;
            if(mostra_info_artigo(codigo, &quantidade, &preco) != 0) {
                fdb_printf(fdbStderr, "[MOSTRAR_STOCK_E_PRECO] Não foi possível obter as informações do artigo %d!\n");
                break;
            }

            // Elaborar um buffer com a output
            dataSize = sizeof(long) + sizeof(double);
            char data[sizeof(long) + sizeof(double)];
            memcpy(data, &quantidade, sizeof(quantidade));
            memcpy(data + sizeof(quantidade), &preco, sizeof(preco));

            result = data;
        } else if(instruction == SV_INSTRUCTION_ATUALIZAR_STOCK_E_MOSTRAR_NOVO_STOCK) {
            // Esta instrução lê do pipe os seguintes argumentos:
            // long => codigo
            // long => quantidade
            // pid_t => pid do processo que requereu os dados
            // Esta instrução escreve no pipe de resposta os seguintes argumentos:
            // long => novoStock

            long codigo;
            if(fdb_read(fdbFifo, &codigo, sizeof(codigo)) < 0) {
                fdb_printf(fdbStderr, "[ATUALIZAR_STOCK_E_MOSTRAR_NOVO_STOCK] Erro ao ler o código do artigo do FIFO! Terminando...\n");
                break;
            }

            long quantidade;
            if(fdb_read(fdbFifo, &quantidade, sizeof(quantidade)) < 0) {
                fdb_printf(fdbStderr, "[ATUALIZAR_STOCK_E_MOSTRAR_NOVO_STOCK] Erro ao ler a quantidade do FIFO! Terminando...\n");
                break;
            }

            long novoStock;
            if(atualiza_mostra_stock(codigo, quantidade, &novoStock) != 0) {
                fdb_printf(fdbStderr, "[ATUALIZAR_STOCK_E_MOSTRAR_NOVO_STOCK] Não foi possível atualizar o stock do artigo! Terminando...\n");
                break;
            }

            // Elaborar um buffer com a output
            dataSize = sizeof(long);
            result = (char *) &novoStock;
        } else if(instruction == SV_INSTRUCTION_EXECUTAR_AG) {
            // TODO: Mandar executar o AG!
        }

        // Obter o PID do processo a partir da pipe
        pid_t requesterPid;
        if(fdb_read(fdbFifo, &requesterPid, sizeof(requesterPid)) < 0) {
            fdb_printf(fdbStderr, "Erro ao ler o PID do FIFO! Terminando...\n");
            break;
        }

        char requesterPidStr[sizeof(pid_t)];
        sprintf(requesterPidStr, "%d", requesterPid);

        // Usando o ID do processo, podemos agora abrir a pipe de output deste processo,
        // e escrever o resultado da operação para essa pipe
        fdb_t outPipe;
        file_open(&outPipe, requesterPidStr, false, true);
        fdb_write(outPipe, result, dataSize);
        file_close(outPipe);
    }

    // Fechar os ficheiros abertos
    file_close(g_pFdbVendas);
    file_close(g_pFdbStocks);
    file_close(g_pFdbArtigos);
    file_close(g_pFdbStrings);

    // Fechar a fifo de comunicação
    file_close(fdbFifo);
    unlink(SV_FIFO_NAME);

    // Fechar os fdb_ts do stdin, stdout e stderr
    fdb_destroy(fdbStderr);
    fdb_destroy(fdbStdout);
    fdb_destroy(fdbStdin);

    // Sucesso
    return 0;
}
