#ifndef SO1819_STOCK_H
#define SO1819_STOCK_H

#include <stdio.h>

#include "artigo.h"

/** @brief Define o nome do ficheiro de stocks a utilizar. */
#define NOME_FICHEIRO_STOCKS "STOCKS"

/** @brief Guarda uma referência ao fdbuffer que lê e escreve para o ficheiro dos STOCKS. */
extern fdb_t g_pFdbStocks;

/**
 * @brief Estrutura para guardar uma entrada do ficheiro de STOCKS.
 */
typedef struct stock {
    /** @brief Offset desta entrada no ficheiro de STOCKS. */
    ssize_t offset;
    /** @brief O código do artigo ao qual se refere este stock. */
    long codigoArtigo;
    /** @brief A quantidade do artigo atualmente em stock. */
    long quantidade;
} *stock_t;

/** @brief Calcula o tamanho de uma entrada no ficheiro STOCK de uma única entrada de stock, para ser fácil calcular o offset a partir de um código de um artigo. */
#define TAMANHO_ENTRADA_STOCK (sizeof(long) + sizeof(long))

/** @brief Define o offset no ficheiro STOCKS onde aparece a primeira entrada do stock de um artigo. */
#define INICIO_ENTRADAS_STOCK 0

/**
 * @brief Cria uma nova estrutura em memória para a respetiva entrada de stock.
 *
 * @param codigoArtigo O código do sartigo ao qual esta entrada de stock se refere
 * @param quantidade A quantidade de stock efetivamente disponível para este artigo
 *
 * @return A estrutura de stock criada, ou NULL em caso de erro
 */
stock_t stock_new(long codigoArtigo, long quantidade);

/**
 * @brief Liberta a memória utilizada por esta estrutura de stock.
 *
 * @param stock A estrutura de stock para a qual libertar a memória
 */
void stock_free(stock_t stock);

/**
 * @brief Carrega uma entrada de stock a partir do ficheiro de STOCKS.
 *
 * Utiliza o parâmetro {@param codigoArtigo} para determinar qual o offset a carregar.
 *
 * @param codigoArtigo O código do artigo cujo stock deve ser carregado
 * @param stock Onde guardar o stock carregado
 *
 * @return 0 se correu tudo bem; <0 em caso de erro
 */
int stock_load(long codigoArtigo, stock_t *stockRef);

/**
 * @brief Guarda uma entrada de stock no ficheiro de STOCKS.
 *
 * Utiliza o campo offset para saber qual o offset em que guardar a entrada.
 *
 * @param stock A entrada de stock a guardar no ficheiro
 *
 * @return 0 se correu tudo bem; <0 em caso de erro
 */
int stock_save(stock_t stock);

#endif //SO1819_STOCK_H
