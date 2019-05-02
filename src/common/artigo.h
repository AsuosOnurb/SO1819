#ifndef SO1819_ARTIGO_H
#define SO1819_ARTIGO_H

#include "strings.h"

/** @brief Define o nome do ficheiro de artigos a utilizar. */
#define NOME_FICHEIRO_ARTIGOS "ARTIGOS"

/**
 * @brief Define a estrutura de um artigo.
 */
typedef struct artigo {
    /** @brief O offset no ficheiro de ARTIGOS onde este artigo está guardado. -1 se este artigo ainda não está guardado ou a sua localização é desconhecida. */
    ssize_t offset;
    /** @brief Referência à string que é o nome do artigo. */
    string_t nome;
    /** @brief Código do artigo. */
    int codigo;
    /** @brief Preço do artigo. */
    int preco;
} *artigo_t;

/**
 * @brief Cria um novo artigo em memória.
 *
 * @param nome Referência (possivelmente com o campo char* string por preencher) à string que é o nome do artigo
 * @param codigo Código do artigo a criar
 * @param preco Preço por unidade do artigo
 *
 * @return O artigo criado
 */
artigo_t artigo_new(string_t nome, int codigo, int preco);

/**
 * @brief Liberta a memória utilizada pela estrutura do artigo referenciada.
 *
 * @param artigo O artigo a libertar
 */
void artigo_free(artigo_t artigo);

/**
 * @brief Lê um artigo a partir do ficheiro ARTIGOS.
 * Utiliza o campo offset, que se assume pré-preenchido, para saber qual o offset a carregar.
 * Em alternativa, pode utilizar o campo codigo, no caso de offset == -1, para saber qual o offset a carregar.
 *
 * @param artigo Onde guardar o artigo carregado
 *
 * @return 0 em caso de sucesso, <0 em caso de erro
 */
int artigo_load(artigo_t *artigo);

/**
 * @brief Guarda um artigo para o ficheiro ARTIGOS.
 *
 * @param artigo O artigo a guarar
 *
 * @return 0 se tudo correu bem; <0 em caso de erro
 */
int artigo_save(artigo_t artigo);

#endif //SO1819_ARTIGO_H
