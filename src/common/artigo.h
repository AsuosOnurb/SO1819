#ifndef SO1819_ARTIGO_H
#define SO1819_ARTIGO_H

#include "strings.h"

/** @brief Define o nome do ficheiro de artigos a utilizar. */
#define NOME_FICHEIRO_ARTIGOS "ARTIGOS"

/** @brief Guarda uma referência ao fdbuffer que lê e escreve para o ficheiro dos ARTIGOS. */
extern fdb_t g_pFdbArtigos;

/**
 * @brief Define a estrutura de um artigo.
 */
typedef struct artigo {
    /** @brief O offset no ficheiro de ARTIGOS onde este artigo está guardado. -1 se este artigo ainda não está guardado ou a sua localização é desconhecida. */
    ssize_t offset;
    /** @brief Código do artigo. */
    long codigo;
    ///** @brief Referência à string que é o nome do artigo. */
    //string_t nome;
    /** @brief Posição no ficheiro de STRINGS que contém o nome deste artigo. */
    ssize_t offsetNome;
    /** @brief Preço do artigo. */
    double preco;
} *artigo_t;

/** @brief Calcula o tamanho de uma entrada no ficheiro ARTIGOS de um único artigo, para ser fácil calcular o offset a partir de um código de um artigo. */
// #define TAMANHO_ENTRADA_ARTIGO (sizeof(((artigo_t) NULL)->codigo) + sizeof(((artigo_t) NULL)->nome->offset) + sizeof(((artigo_t) NULL)->preco))
// #define TAMANHO_ENTRADA_ARTIGO (sizeof(((artigo_t) NULL)->codigo) + sizeof(((artigo_t) NULL)->offsetNome) + sizeof(((artigo_t) NULL)->preco))
#define TAMANHO_ENTRADA_ARTIGO (sizeof(ssize_t) + sizeof(double))

/** @brief Define o offset no ficheiro ARTIGOS onde aparece a primeira entrada de um artigo. */
#define INICIO_ENTRADAS_ARTIGO 0

/**
 * @brief Guarda o próximo código de artigo que pode ser utilizado.
 *
 * Deve, obrigatóriamente, ter o mesmo tipo de dados do que o field código da estrutura artigo.<br>
 * Deve ser incrementado quando é adicionado um artigo NOVO ao ficheiro.<br>
 * É automaticamente inicializada quando necessário.<br>
 */
extern long g_iProximoCodigoUtilizavel;

/**
 * @brief Função responsável por inicializar a partir do disco o ficheiro de ARTIGOS.<br>
 * É também responsável por obter a partir do ficheiro, aquando da sua inicialização,
 * o próximo código disponível para utilizar por um novo artigo que poderá vir a ser criado.<br>
 *
 * O próximo código utilizável é calculado fazendo lseek() para a última entrada do ficheiro,
 * e lendo o código do último artigo inserido no ficheiro, e somando-lhe uma unidade.
 *
 * @return 0 em caso de sucesso, <0 em caso de erro
 */
int inicializar_ficheiro_artigos();

/**
 * @brief Cria um novo artigo em memória.
 *
 * @param codigo Código do artigo a criar
 * @param offsetNome Offset no ficheiro de STRINGS referente ao nome deste artigo
 * @param preco Preço por unidade do artigo
 *
 * @return O artigo criado
 */
artigo_t artigo_new(long codigo, ssize_t offsetNome, double preco);

/**
 * @brief Liberta a memória utilizada pela estrutura do artigo referenciada.
 *
 * @param artigo O artigo a libertar
 */
void artigo_free(artigo_t artigo);

/**
 * @brief Lê um artigo a partir do ficheiro ARTIGOS.
 *
 * @param codigo Código do artigo a carregar
 * @param artigo Onde guardar o artigo carregado
 *
 * @return 0 em caso de sucesso, <0 em caso de erro
 */
int artigo_load(long codigo, artigo_t *artigo);

/**
 * @brief Guarda um artigo para o ficheiro ARTIGOS.
 *
 * Se o código do artigo fornecido for -1, então atribui um novo código ao artigo antes de guardar.<br>
 * Se o offset do artigo fornecido for -1, então calcula o offset correto a partir do código do artigo.
 *
 * @param artigo O artigo a guarar
 *
 * @return 0 se tudo correu bem; <0 em caso de erro
 */
int artigo_save(artigo_t artigo);

#endif //SO1819_ARTIGO_H
