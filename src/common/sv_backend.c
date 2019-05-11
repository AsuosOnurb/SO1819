#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <zconf.h>
#include "sv_backend.h"
#include "fdb.h"
#include "util.h"

fdb_t g_pFdbServerFifo = NULL;

int sv_send_instruction(instruction_t instructionCode, const void *params, size_t dataSize, pid_t currentPid) {
    // First, make sure we have a communication pipe with the server
    fdb_t pFdbServerFifo;
    // Attempt to open the pipe
    if(file_open(&pFdbServerFifo, SV_FIFO_NAME, false, true) != 0)
        return -1; // error!


    // POSIX only guarantees that a single write less than PIPE_BUF bytes to a fifo won't be interlaced with other writes from other processes
    // So, let's ensure we do a single write call by creating a buffer and writting to it
    char *buf = (char *) malloc(sizeof(instructionCode) + dataSize + sizeof(currentPid));

    // Copy to the buffer the instruction code
    memcpy(buf, &instructionCode, sizeof(instructionCode));
    // Copy to the buffer the parameters of the instruction
    memcpy(buf + sizeof(instructionCode), params, dataSize);
    // Copy to the buffer the PID of the process so that the server knows who asked and knows where to direct its answer
    memcpy(buf + sizeof(instructionCode) + dataSize, &currentPid, sizeof(currentPid));

    // Now, attempt to actually write everything into the buffer
    if(fdb_write(pFdbServerFifo, buf, sizeof(instructionCode) + dataSize + sizeof(currentPid)) != 0) {
        free(buf);
        return -3;
    }

    // Free the buffer memory
    free(buf);

    // Reading the response is the responsibility of each instruction's own code

    // Sucesso!
    return 0;
}

int sv_get_info_artigo(long codigoArtigo, long *quantidade, double *preco) {
    // Esta instrução escreve no pipe do servidor os seguintes argumentos:
    // long => codigo
    // pid_t => pid do processo atual, que requereu os dados

    char params[sizeof(codigoArtigo)];
    memcpy(params, &codigoArtigo, sizeof(codigoArtigo));

    // O PID atual vai servir para identificar a fifo a partir da qual o servidor vai responder ao cliente
    pid_t pid = getpid();

    // Obter o PID do processo como string
    char pidStr[128];
    sprintf(pidStr, "%d", pid);

    // Abrir um file descriptor para a fifo de resposta, que automaticamente cria essa mesma fifo
    fdb_t fdbResponseFifo;
    if(fdb_mkfifo(&fdbResponseFifo, pidStr, O_RDONLY, 0644) != 0)
        return -1;

    // Enviar ao servidor a instrução
    if(sv_send_instruction(SV_INSTRUCTION_MOSTRAR_STOCK_E_PRECO, params, sizeof(params), pid) != 0)
        return -2;

    // Read the response as soon as it becomes available
    // Esta instrução lê do pipe de resposta os seguintes argumentos:
    // long => quantidade
    // double => preço
    if(fdb_read(fdbResponseFifo, quantidade, sizeof(*quantidade)) <= 0)
        return -3;

    if(fdb_read(fdbResponseFifo, preco, sizeof(*preco)) <= 0)
        return -4;

    // Fechar o file descriptor da fifo de resposta
    if(fdb_unlink(fdbResponseFifo) != 0)
        return -4;

    // Sucesso!
    return 0;
}

int sv_update_mostra_stock(long codigoArtigo, long acrescento, long *novoStock) {
    // Esta instrução escreve no pipe os seguintes argumentos:
    // long => codigo
    // long => acrescento/quantidade
    // pid_t => pid do processo que requereu os dados

    char params[sizeof(codigoArtigo) + sizeof(acrescento)];
    memcpy(params, &codigoArtigo, sizeof(codigoArtigo));

    // O PID atual vai servir para identificar a fifo a partir da qual o servidor vai responder ao cliente
    pid_t pid = getpid();

    // Obter o PID do processo como string
    char pidStr[128];
    sprintf(pidStr, "%d", pid);

    // Abrir um file descriptor para a fifo de resposta, que automaticamente cria essa mesma fifo
    fdb_t fdbResponseFifo;
    if(fdb_mkfifo(&fdbResponseFifo, pidStr, O_RDONLY, 0644) != 0)
        return -1;

    // Enviar ao servidor a instrução
    if(sv_send_instruction(SV_INSTRUCTION_ATUALIZAR_STOCK_E_MOSTRAR_NOVO_STOCK, params, sizeof(params), pid) != 0)
        return -2;

    // Read the response as soon as it becomes available
    // Esta instrução lê do pipe de resposta os seguintes argumentos:
    // long => novoStock
    if(fdb_read(fdbResponseFifo, novoStock, sizeof(*novoStock)) <= 0)
        return -3;

    // Fechar o file descriptor da fifo de resposta
    if(fdb_unlink(fdbResponseFifo) != 0)
        return -4;

    // Sucesso!
    return 0;
}