#ifndef SO1819_STOCK_H
#define SO1819_STOCK_H

#include <stdio.h>

#include "artigo.h"

/** @brief Define o nome do ficheiro de stocks a utilizar. */
#define NOME_FICHEIRO_STOCKS "STOCKS"

/**
 * @brief Estrutura para guardar uma entrada do ficheiro de STOCKS.
 */
typedef struct stock {
    /** @brief Offset desta entrada no ficheiro de STOCKS. */
    ssize_t offset;
    /** @brief O artigo ao qual se refere este stock. */
    artigo_t artigo;
    /** @brief A quantidade do artigo atualmente em stock. */
    int quantidade;
} *stock_t;

/**
 * @brief Cria uma nova estrutura em memória para a respetiva entrada de stock.
 *
 * @param offset O offset onde está guardada esta entrada em disco
 * @param artigo O artigo ao qual esta entrada de stock se refere
 * @param quantidade A quantidade de stock efetivamente disponível para este artigo
 *
 * @return A estrutura de stock criada, ou NULL em caso de erro
 */
stock_t stock_new(ssize_t offset, artigo_t artigo, int quantidade);

/**
 * @brief Liberta a memória utilizada por esta estrutura de stock.
 *
 * @param stock A estrutura de stock para a qual libertar a memória
 */
void stock_free(stock_t stock);

/**
 * @brief Carrega uma entrada de stock a partir do ficheiro de STOCKS.
 *
 * Utiliza o parâmetro {@param codigo} do artigo dado para determinar qual o offset a carregar.
 *
 * @param artigo O artigo cujo stock deve ser carregado
 * @param stock Onde guardar o stock carregado
 *
 * @return 0 se correu tudo bem; <0 em caso de erro
 */
int stock_load(artigo_t artigo, stock_t *stock);

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
