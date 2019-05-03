#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>

#include "fdb.h"

#include "util.h"
#include "strings.h"

fdb_t g_pFdbStrings = NULL;

int string_load(size_t offset, string_t *strRef) {
    // Assumimos um offset válido!
    if(strRef == NULL)
        return -1;

    // Verificar se o ficheiro está aberto
    if(g_pFdbStrings == NULL)
        if(file_open(&g_pFdbStrings, NOME_FICHEIRO_STRINGS, 1) != 0)
            return -2;

    // Carregar efetivamente a string
    if(fdb_lseek(g_pFdbStrings, offset, SEEK_SET) != offset)
        return -3;

    // Ler o tamanho da string a partir do ficheiro
    size_t length = 0;
    if(fdb_read(g_pFdbStrings, &length, sizeof(size_t)) <= 0)
        return -4;

    // Verificar se o tamanho é válido
    if(length <= 0)
        return -5;

    // Alocar espaço para a string
    char *string = (char *) malloc(sizeof(char) * length + 1);
    if(fdb_read(g_pFdbStrings, string, length) != length)
        return -6;
    string[length] = '\0'; // Terminar a string com NULL, pois este byte não é guardado no disco

    // Criar uma referência para a string
    *strRef = (string_t) malloc(sizeof(struct string_ref));
    (*strRef)->offset = offset;
    (*strRef)->length = length;
    (*strRef)->string = string;

    // Retornar sucesso
    return 0;
}

int string_save(const char *string, string_t *strRef) {
    // Verificar parâmetros
    if(string == NULL)
        return -1;

    if(strRef == NULL)
        return -2;

    // Verificar se o ficheiro está aberto
    if(g_pFdbStrings == NULL)
        if(file_open(&g_pFdbStrings, NOME_FICHEIRO_STRINGS, 1) != 0)
            return -3;

    // Guardar efetivamente a string
    // Calcular o offset onde a string vai ser escrita
    off_t offset = fdb_lseek(g_pFdbStrings, 0, SEEK_END);
    if(offset < 0)
        return -4;

    // Escrever o tamanho da string
    size_t length = strlen(string);
    if(fdb_write(g_pFdbStrings, &length, sizeof(size_t)) != 0)
        return -5;

    // Escrever a string
    if(fdb_write(g_pFdbStrings, string, length) != 0)
        return -6;

    // Criar uma referência para a string,
    // fazendo uma cópia da string, pois nós queremos controlar
    // todas as referências de modo a podermos fazer memory management
    // de strings automaticamente e apenas dentro deste ficheiro
    *strRef = (string_t) malloc(sizeof(struct string_ref));
    (*strRef)->offset = offset;
    (*strRef)->length = length;
    (*strRef)->string = (const char *) malloc(sizeof(char) * length);
    memcpy((char *) (*strRef)->string, string, length);

    // Retornar sucesso
    return 0;
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
