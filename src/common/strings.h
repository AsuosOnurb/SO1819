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
    /** @brief Flag que marca se a string está atualmente em uso (caso em que tem valor 1), ou se pode ser apagada do ficheiro STRINGS sem consequências (caso em que tem valor 0). */
    int usada;
    /** @brief Valor efetivo da string, ou NULL se ainda não tiver sido preenchida. */
    char *string;
} *string_t;

/**
 * @brief Lê o valor efetivo da string a partir do ficheiro STRINGS.
 *
 * @param offset O offset onde está guardada a string no ficheiro de STRINGS
 *
 * @return A string preenchida, ou NULL em caso de erro
 */
string_t string_load(size_t offset);

/**
 * Escreve uma string para o ficheiro de STRINGS.
 *
 * @param string A string a guardar no ficheiro
 *
 * @return A estrutura referente à string guardada
 */
string_t string_save(char *string);

/**
 * @brief Declara que a string fornecida pode ser eliminada com segurança, pois já não é mais utilizada.
 * É inseguro a utilização da estrutura string_t passada como argumento após esta função ser executada, pois esta pode ser eliminada a qualquer momento.
 *
 * @param offset O offset da string a eliminar
 */
void string_delete(string_t string);

/**
 * Splits a single string by the delimiter sep, returning an array of arrays for each token in the input string.
 *
 * @param str The string to parse
 * @param sep The delimiter to split by
 * @param argvout Where to put the address of **argv
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
