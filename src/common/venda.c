#include <stdlib.h>

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

    // Carregar o código do artigo vendido
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
    }

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
        }
    } else {
        if(fdb_lseek(g_pFdbVendas, venda->offset, SEEK_SET) != venda->offset)
            return -4;
    }

    // Efetivamente guardar a venda no disco

    // Guardar o código do artigo vendido
    if(fdb_write(g_pFdbVendas, &venda->codigo, sizeof(venda->codigo)) != 0)
        return -5;

    // Guardar a quantidade total de artigos vendidos
    if(fdb_write(g_pFdbVendas, &venda->quantidade, sizeof(venda->quantidade)) != 0)
        return -6;

    // Guardar o montante total da venda
    if(fdb_write(g_pFdbVendas, &venda->montante, sizeof(venda->montante)) != 0)
        return -7;

    // Sucesso!
    return 0;
}
