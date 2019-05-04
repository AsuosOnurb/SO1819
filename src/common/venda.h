#ifndef SO1819_VENDA_H
#define SO1819_VENDA_H

#include <stdio.h>

/** @brief Define o nome do ficheiro de vendas a utilizar. */
#define NOME_FICHEIRO_VENDAS "VENDAS"

/**
 * @brief Define a estrutura de uma venda.
 */
typedef struct venda {
    /** @brief O offset no ficheiro de VENDAS onde esta venda está guardado. -1 se esta venda ainda não está guardada ou a sua localização é desconhecida. */
    ssize_t offset;
    /** @brief Código do artigo. */
    int codigo;
    /** @brief Número de artigos vendidos. */
    int quantidade;
    /** @brief Montante total da venda. */
    int montante;
} *venda_t;

/**
 * @brief Cria uma nova venda em memória.
 *
 * @param codigo Código do artigo
 * @param quantidade Número de artigos vendidos
 * @param montante Montante total da venda
 *
 * @return O artigo criado
 */
venda_t venda_new(int codigo, int quantidade, int montante);

/**
 * @brief Liberta a memória utilizada pela estrutura da venda referenciada.
 *
 * @param venda A venda a libertar
 */
void venda_free(venda_t venda);

/**
 * @brief Lê uma venda a partir do ficheiro VENDAS.
 * Utiliza o campo offset, que se assume pré-preenchido, para saber qual o offset a carregar.
 *
 * @param venda Onde guardar os dados da venda carregados
 *
 * @return 0 em caso de sucesso, <0 em caso de erro
 */
int venda_load(venda_t *venda);

/**
 * @brief Guarda uma venda para o ficheiro VENDAS.
 *
 * @param venda A venda a guardar
 *
 * @return 0 se tudo correu bem; <0 em caso de erro
 */
int venda_save(venda_t venda);

#endif //SO1819_VENDA_H
