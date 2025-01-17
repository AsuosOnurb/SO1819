#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#include <unistd.h>
#include <fcntl.h>

#include "fdb.h"
#include "util.h"

#include "sv_protocol.h"

fdb_t g_pFdbServerFifo = NULL;

int sv_send_instruction(instruction_t instructionCode, const char *params, size_t dataSize, pid_t currentPid) {
    // First, make sure we have a communication pipe with the server
    if(g_pFdbServerFifo == NULL)
        // Attempt to open the pipe
        if(file_open(&g_pFdbServerFifo, SV_FIFO_NAME, false, true) != 0)
            return -1; // error!


    // POSIX only guarantees that a single write less than PIPE_BUF bytes to a fifo won't be interlaced with other writes from other processes
    // So, let's ensure we do a single write call by creating a buffer and writing to it
    char *buf = (char *) malloc(sizeof(currentPid) + sizeof(instructionCode) + dataSize);

    // Copy to the buffer the instruction code
    memcpy(buf, &instructionCode, sizeof(instructionCode));
    // Copy to the buffer the PID of the process so that the server knows who asked and knows where to direct its answer
    memcpy(buf + sizeof(instructionCode), &currentPid, sizeof(currentPid));
    // Copy to the buffer the parameters of the instruction
    memcpy(buf + sizeof(instructionCode) + sizeof(currentPid), params, dataSize);

    // Now, attempt to actually write everything into the buffer
    if(fdb_write(g_pFdbServerFifo, buf, sizeof(instructionCode) + sizeof(currentPid) + dataSize) != 0) {
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
    calcularFifoResposta(pid, pidStr);

    // Abrir um file descriptor para a fifo de resposta, que automaticamente cria essa mesma fifo
    fdb_t fdbResponseFifo;
    if(fdb_mkfifo(&fdbResponseFifo, pidStr, O_RDONLY, 0644) != 0)
        return -1;

    // Enviar ao servidor a instrução
    if(sv_send_instruction(SV_INSTRUCTION_MOSTRAR_STOCK_E_PRECO, params, sizeof(params), pid) != 0) {
        fdb_unlink(fdbResponseFifo);
        return -2;
    }

    // Read the response as soon as it becomes available

    // Verificar, antes de tudo, se a resposta indica que foi bem sucedida
    bool sucesso;
    if(fdb_read(fdbResponseFifo, &sucesso, sizeof(sucesso)) <= 0) {
        fdb_unlink(fdbResponseFifo);
        return -3;
    }

    // Verificar se a execução da instrução foi bem sucedida
    if(!sucesso) {
        fdb_unlink(fdbResponseFifo);
        return -4;
    }

    // Esta instrução lê do pipe de resposta os seguintes argumentos:
    // long => quantidade
    // double => preço
    if(fdb_read(fdbResponseFifo, quantidade, sizeof(*quantidade)) <= 0) {
        fdb_unlink(fdbResponseFifo);
        return -5;
    }

    if(fdb_read(fdbResponseFifo, preco, sizeof(*preco)) <= 0) {
        fdb_unlink(fdbResponseFifo);
        return -6;
    }

    // Fechar o file descriptor da fifo de resposta
    if(fdb_unlink(fdbResponseFifo) != 0)
        return -7;

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
    memcpy(params + sizeof(codigoArtigo), &acrescento, sizeof(acrescento));

    // O PID atual vai servir para identificar a fifo a partir da qual o servidor vai responder ao cliente
    pid_t pid = getpid();

    // Obter o PID do processo como string
    char pidStr[128];
    calcularFifoResposta(pid, pidStr);

    // Abrir um file descriptor para a fifo de resposta, que automaticamente cria essa mesma fifo
    fdb_t fdbResponseFifo;
    if(fdb_mkfifo(&fdbResponseFifo, pidStr, O_RDONLY, 0644) != 0)
        return -1;

    // Enviar ao servidor a instrução
    if(sv_send_instruction(SV_INSTRUCTION_ATUALIZAR_STOCK_E_MOSTRAR_NOVO_STOCK, params, sizeof(params), pid) != 0) {
        fdb_unlink(fdbResponseFifo);
        return -2;
    }

    // Read the response as soon as it becomes available
    bool sucesso;
    if(fdb_read(fdbResponseFifo, &sucesso, sizeof(sucesso)) <= 0) {
        fdb_unlink(fdbResponseFifo);
        return -3;
    }

    // Verificar se a execução da instrução foi bem sucedida
    if(!sucesso) {
        fdb_unlink(fdbResponseFifo);
        return -4;
    }

    // Esta instrução lê do pipe de resposta os seguintes argumentos:
    // long => novoStock
    if(fdb_read(fdbResponseFifo, novoStock, sizeof(*novoStock)) <= 0) {
        fdb_unlink(fdbResponseFifo);
        return -5;
    }

    // Fechar o file descriptor da fifo de resposta
    if(fdb_unlink(fdbResponseFifo) != 0)
        return -6;

    // Sucesso!
    return 0;
}