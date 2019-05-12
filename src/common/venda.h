#ifndef SO1819_VENDA_H
#define SO1819_VENDA_H

#include <stdio.h>

#include "fdb.h"

/** @brief Define o nome do ficheiro de vendas a utilizar. */
#define NOME_FICHEIRO_VENDAS "VENDAS.txt"

/** @brief Guarda uma referência ao fdbuffer que lê e escreve para o ficheiro dos VENDAS. */
extern fdb_t g_pFdbVendas;

/**
 * @brief Define a estrutura de uma venda.
 */
typedef struct venda {
    /** @brief O offset no ficheiro de VENDAS onde esta venda está guardado. -1 se esta venda ainda não está guardada ou a sua localização é desconhecida. */
    ssize_t offset;
    /** @brief Tamanho da venda no disco. */
    ssize_t diskSize;
    /** @brief Código do artigo. */
    long codigo;
    /** @brief Número de artigos vendidos. */
    long quantidade;
    /** @brief Montante total da venda. */
    double montante;
} *venda_t;

/** @brief Calcula o tamanho de uma entrada no ficheiro VENDAS de uma única entrada de stock, para ser fácil calcular o offset a partir de um código de um artigo. */
// #define TAMANHO_ENTRADA_VENDA (sizeof(long) + sizeof(long) + sizeof(long))

/** @brief Define o offset no ficheiro STOCKS onde aparece a primeira entrada do stock de um artigo. */
#define INICIO_ENTRADAS_VENDA 1024
// (sizeof(long))


/**
 * @brief Cria uma nova venda em memória.
 *
 * @param codigo Código do artigo
 * @param quantidade Número de artigos vendidos
 * @param montante Montante total da venda
 *
 * @return O artigo criado
 */
venda_t venda_new(long codigo, long quantidade, double montante);

/**
 * @brief Liberta a memória utilizada pela estrutura da venda referenciada.
 *
 * @param venda A venda a libertar
 */
void venda_free(venda_t venda);

/**
 * @brief Lê uma venda a partir do ficheiro VENDAS.
 *
 * Utiliza o campo offset, que se assume pré-preenchido, para saber qual o offset a carregar.
 *
 * @param offset O offset de onde carregar a respetiva venda
 * @param venda Onde guardar os dados da venda carregados
 *
 * @return 0 em caso de sucesso, <0 em caso de erro
 */
int venda_load(ssize_t offset, venda_t *venda);

/**
 * @brief Guarda uma venda para o ficheiro VENDAS.
 *
 * @param venda A venda a guardar
 *
 * @return 0 se tudo correu bem; <0 em caso de erro
 */
int venda_save(venda_t venda);

#endif //SO1819_VENDA_H
