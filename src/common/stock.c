#include <stdlib.h>
#include "stock.h"
#include "util.h"

fdb_t g_pFdbStocks = NULL;

stock_t stock_new(long codigoArtigo, long quantidade) {
    // Criar a estrutura com o stock
    stock_t stock = (stock_t) malloc(sizeof(struct stock));

    stock->offset = TAMANHO_ENTRADA_STOCK * codigoArtigo + INICIO_ENTRADAS_STOCK;
    stock->codigoArtigo = codigoArtigo;
    stock->quantidade = quantidade;

    return stock;
}

void stock_free(stock_t stock) {
    // Verificar parâmetros
    if(stock == NULL)
        return;

    // Libertar a memória do stock
    free(stock);
}

int stock_load(long codigoArtigo, stock_t *stockRef) {
    // Verificar parâmetros
    if(codigoArtigo < 0)
        return -1;

    if(stockRef == NULL)
        return -2;

    // Verificar se o ficheiro dos stocks está aberto
    if(g_pFdbStocks == NULL)
        if(file_open(&g_pFdbStocks, NOME_FICHEIRO_STOCKS, 1) != 0)
            return -3;

    // Criar uma entrada em memória para este stock, que automaticamente calcula o offset onde esta se deve encontrar
    stock_t stock = stock_new(codigoArtigo, -1);

    // Mover o apontador do ficheiro para a posição correta
    if(fdb_lseek(g_pFdbStocks, stock->offset, SEEK_SET) != stock->offset)
        return -4;

    // Ler do ficheiro a quantidade deste artigo disponível em stock
    if(fdb_read(g_pFdbStocks, &stock->quantidade, sizeof(stock->quantidade)) <= 0)
        return -5;

    // Sucesso!
    *stockRef = stock;
    return 0;
}

int stock_save(stock_t stock) {
    // Verificar parâmetros
    if(stock == NULL)
        return -1;

    // Verificar se o ficheiro dos stocks está aberto
    if(g_pFdbStocks == NULL)
        if(file_open(&g_pFdbStocks, NOME_FICHEIRO_STOCKS, 1) != 0)
            return -2;

    // Mover o apontador do ficheiro para a posição correta
    if(fdb_lseek(g_pFdbStocks, stock->offset, SEEK_SET) != stock->offset)
        return -3;

    // Guardar no ficheiro a quantidade deste artigo disponível em stock
    if(fdb_write(g_pFdbStocks, &stock->quantidade, sizeof(stock->quantidade)) != 0)
        return -4;

    // Sucesso!
    return 0;
}
