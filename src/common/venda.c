#include <stdlib.h>
#include <string.h>

#include "util.h"

#include "venda.h"

fdb_t g_pFdbVendas = NULL;

venda_t venda_new(long codigo, long quantidade, double montante) {
    // Criar a estrutura com a venda
    venda_t venda = (venda_t) malloc(sizeof(struct venda));

    venda->offset = -1;
    venda->codigo = codigo;
    venda->quantidade = quantidade;
    venda->montante = montante;

    return venda;
}

void venda_free(venda_t venda) {
    // Verificar parâmetros
    if(venda == NULL)
        return;

    // Efetivamente libertar a memória
    free(venda);
}

int venda_load(ssize_t offset, venda_t *vendaRef) {
    // Verificar parâmetros
    if(offset < 0)
        return -1;

    if(vendaRef == NULL)
        return -2;

    // Verificar se o ficheiro de vendas está aberto
    if(g_pFdbVendas == NULL)
        if(file_open(&g_pFdbVendas, NOME_FICHEIRO_VENDAS, true, true) != 0)
            return -3;

    // Fazer seek para a posição correta
    if(fdb_lseek(g_pFdbVendas, offset, SEEK_SET) != offset)
        return -4;

    // Criar uma nova estrutra para guardar os dados da venda
    venda_t venda = venda_new(-1, -1, -1);

    // Inicializar o offset
    venda->offset = offset;

    // Carregar os dados do disco

    /*// Carregar o código do artigo vendido
    if(fdb_read(g_pFdbVendas, &venda->codigo, sizeof(venda->codigo)) <= 0) {
        venda_free(venda);
        return -5;
    }

    // Carregar a quantidade vendida
    if(fdb_read(g_pFdbVendas, &venda->quantidade, sizeof(venda->quantidade)) <= 0) {
        venda_free(venda);
        return -6;
    }

    // Carregar o montante total da venda
    if(fdb_read(g_pFdbVendas, &venda->montante, sizeof(venda->montante)) <= 0) {
        venda_free(venda);
        return -7;
    }*/

    size_t bufferSize = 512, start = 0;
    char *line = (char *) malloc(sizeof(char) * bufferSize);

    ssize_t bytesRead;
    while((bytesRead = fdb_readln(g_pFdbVendas, line + start, bufferSize)) > 0) {
        // Make sure we read a full line
        if(line[bytesRead - 2] != '\n') {
            start = bufferSize;
            bufferSize *= 2;
            line = realloc(line, sizeof(char) * bufferSize);
            continue;
        } else break;
    }

    venda->diskSize = strlen(line) + 1;

    // Interpretar a linha
    sscanf(line, "%ld %ld %lf\n", &venda->codigo, &venda->quantidade, &venda->montante);

    free(line);

    // Sucesso!
    *vendaRef = venda;
    return 0;
}

int venda_save(venda_t venda) {
    // Verificar parâmetro
    if(venda == NULL)
        return -1;

    // Verificar se o ficheiro de vendas está aberto
    if(g_pFdbVendas == NULL)
        // Tentar abrir o ficheiro
        if(file_open(&g_pFdbVendas, NOME_FICHEIRO_VENDAS, true, true) != 0)
            return -2;

    // Fazer lseek para a posição correta, que depende do offset
    if(venda->offset == -1) {
        // Se offset == -1, então queremos guardar uma nova entrada. Logo, fazer lseek(..., 0, SEEK_END)
        if((venda->offset = fdb_lseek(g_pFdbVendas, 0, SEEK_END)) < 0) {
            venda->offset = -1;
            return -3;
        } else if(venda->offset == 0) {
            // Inicializar o ficheiro de vendas
            static const ssize_t offsetPrimeiraAgregacao = INICIO_ENTRADAS_VENDA;
            if(fdb_write(g_pFdbVendas, &offsetPrimeiraAgregacao, sizeof(offsetPrimeiraAgregacao)) != 0)
                return -4;

            venda->offset += INICIO_ENTRADAS_VENDA;
        }
    } else {
        // if(fdb_lseek(g_pFdbVendas, venda->offset, SEEK_SET) != venda->offset)
        //    return -4;

        // Não é possível guardar vendas que não sejam novas vendas!!!
        return -5;
    }

    // Efetivamente guardar a venda no disco

    /*// Guardar o código do artigo vendido
    if(fdb_write(g_pFdbVendas, &venda->codigo, sizeof(venda->codigo)) != 0)
        return -5;

    // Guardar a quantidade total de artigos vendidos
    if(fdb_write(g_pFdbVendas, &venda->quantidade, sizeof(venda->quantidade)) != 0)
        return -6;

    // Guardar o montante total da venda
    if(fdb_write(g_pFdbVendas, &venda->montante, sizeof(venda->montante)) != 0)
        return -7; */

    // Escrever a venda no disco
    if((venda->diskSize = fdb_printf(g_pFdbVendas, "%ld %ld %lf\n", venda->codigo, venda->quantidade, venda->montante)) < 0)
        return -6;

    // Sucesso!
    return 0;
}
