#ifndef STRING_UTIL_H
#define STRING_UTIL_H

#include "fdb.h"

/** @brief Define o nome do ficheiro de STRINGS a utilizar. */
#define NOME_FICHEIRO_STRINGS "STRINGS"

/** @brief Guarda uma referência ao fdbuffer que lê e escreve para o ficheiro das STRINGS. */
extern fdb_t g_pFdbStrings;

/** @brief Define o início da primeira string presente no ficheiro de strings. */
#define INICIO_FICHEIRO_STRINGS (sizeof(long) << 1)

/** @brief Preenchida automaticamente aquando do carregamento do ficheiro STRINGS, guarda o número total de strings existentes no ficheiro. */
extern long g_iNumeroStringsTotal;
/** @brief Preenchida automaticamente aquando do carregamento do ficheiro STRINGS, guarda o número de strings que foram marcadas para eliminação. */
extern long g_iNumeroStringsMarcadas;

/**
 * @brief Referência a uma string no ficheiro de STRINGS.
 */
typedef struct string_ref {
    /** @brief Offset da string no ficheiro de STRINGS. */
    ssize_t offset;
    /** @brief Tamanho da string. */
    size_t length;
    /** @brief Valor efetivo da string, ou NULL se ainda não tiver sido preenchida. */
    const char *string;
} *string_t;

/**
 * @brief Inicializa o ficheiro de STRINGS e popula automaticamente as variáveis globais a partir do disco.
 *
 * @return 0 em caso de sucesso, <0 em caso de erro
 */
int inicializar_ficheiro_strings();

/**
 * @brief Cria uma nova referência a uma string do ficheiro STRINGS.
 *
 * @param offset O offset onde se encontra esta string
 * @param length O tamanho da string
 * @param str O valor desta string
 *
 * @return A referência criada
 */
string_t string_new(ssize_t offset, size_t length, const char *str);

/**
 * @brief Lê o valor efetivo da string a partir do ficheiro STRINGS.
 *
 * @param offset O offset onde está guardada a string no ficheiro de STRINGS
 * @param strRef Onde guardar a referência da string carregada
 *
 * @return 0 em caso de sucesso, <0 em caso de erro
 */
int string_load(size_t offset, string_t *strRef);

/**
 * @brief Escreve uma string para o ficheiro de STRINGS.
 *
 * @param string A string a guardar no ficheiro
 *
 * @return O offset onde a string ficou guardada no ficheiro
 */
ssize_t string_save(const char *string);

/**
 * @brief Marca uma string para eliminação do ficheiro de STRINGS.
 *
 * @param offset O offset da string a marcar
 *
 * @return 0 se tudo correu bem, <0 em caso de erro
 */
int string_mark(ssize_t offset);

/**
 * @brief Liberta a memória utilizada pela referência de uma string.
 *
 * @param str A referência de string cuja memória deve ser libertada
 */
void string_free(string_t str);

/**
 * @brief Splits a single string by the delimiter sep, returning an array of arrays for each token in the input string.
 *
 * @param str The string to parse
 * @param sep The delimiter to split by
 * @param pArgv Where to put the address of **argv
 *
 * @return The number of strings parsed (aka argc)
 */
size_t str_split(char *str, char sep, char ***pArgv);

/**
 * @brief Frees any dynamically-allocated array of strings.
 * 
 * @param argv The array of strings to free
 */
void free_all(char **argv);

#endif
