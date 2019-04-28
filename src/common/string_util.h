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
