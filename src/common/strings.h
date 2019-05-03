#ifndef STRING_UTIL_H
#define STRING_UTIL_H

/** @brief Define o nome do ficheiro de STRINGS a utilizar. */
#define NOME_FICHEIRO_STRINGS "STRINGS"

/**
 * @brief Referência a uma string no ficheiro de STRINGS.
 */
typedef struct string_ref {
    /** @brief Offset da string no ficheiro de STRINGS. */
    size_t offset;
    /** @brief Tamanho da string. */
    size_t length;
    /** @brief Valor efetivo da string, ou NULL se ainda não tiver sido preenchida. */
    const char *string;
} *string_t;

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
 * Escreve uma string para o ficheiro de STRINGS.
 *
 * @param string A string a guardar no ficheiro
 * @param strRef Onde guardar a referência da string carregada
 *
 * @return A estrutura referente à string guardada
 */
int string_save(const char *string, string_t *strRef);

/**
 * Splits a single string by the delimiter sep, returning an array of arrays for each token in the input string.
 *
 * @param str The string to parse
 * @param sep The delimiter to split by
 * @param pArgv Where to put the address of **argv
 *
 * @return The number of strings parsed (aka argc)
 */
size_t str_split(char *str, char sep, char ***pArgv);

/**
 * Frees any dynamically-allocated array of strings.
 * 
 * @param argv The array of strings to free
 */
void free_all(char **argv);

#endif
