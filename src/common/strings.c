#include <stdlib.h>
#include <string.h>

#include "fdb.c"

#include "strings.h"

/**
 * @brief Abre o ficheiro STRINGS para leitura.
 *
 * @param write 1 se quisermos abrir o ficheiro para escrita, assim como leitura
 * @return 0 se tudo correu bem, <0 em caso de erro
 */
int strings_open(int write) {
    int flags = O_CREAT;

    if(write) flags |= O_RDWR;
    else flags |= O_RDONLY;

    fdb_t fdbuf;
    fdb_fopen(&fdbuf, NOME_FICHEIRO_STRINGS, flags, 0666);
    
}

int strings_fill(string_t *string) {
    // Verificar parâmetros
    if(string == NULL)
        return -1;

    // Efetivamente ler o valor da string e preencher o mesmo
}

size_t str_split(char *str, char sep, char ***pArgv) {
    // Parse arguments and create argc, argv
    char **argv = malloc(sizeof(char *) * 2);
    int maxArgc = 2;
    int argc = 0;

    size_t len = strlen(str);
    size_t i = 0;
    
    char *buf = malloc(len * sizeof(char) + 1);
    size_t bufSize = 0;
    while(i <= len) {
        char c = str[i];

        if(c == sep || c == '\0') {
            buf[bufSize++] = '\0'; // Null-terminate argument string

            // Empty strings should not be part of argv
            if(strlen(buf) != 0) {
                // Ensure argv has enough space available for another pointer
                if(argc + 1 >= maxArgc)
                    argv = realloc(argv, (maxArgc *= 2) * sizeof(char *));

                // Allocate memory for this argument, and copy argument into correct memory location
                argv[argc] = malloc(bufSize * sizeof(char));
                memcpy(argv[argc], buf, bufSize);
                argc++;
            }

            // Reset temporary buffer
            bufSize = 0;
        } else {
            buf[bufSize++] = c;
        }

        i++;
    }

    // Null-terminate the array
    argv[argc] = NULL;

    free(buf);

    *pArgv = argv;
    return argc;
}

void free_all(char **argv) {
    if(argv == NULL)
        return;

    for(int i = 0; argv[i] != NULL; i++) {
        free(argv[i]);
    }

    free(argv);
}
