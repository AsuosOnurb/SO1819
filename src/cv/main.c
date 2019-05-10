#include <stddef.h>
#include <locale.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#include "../common/strings.h"
#include "../common/util.h"
#include "../common/artigo.h"
#include "../ma/ma.h"

#include "cv.h"



int main(int argc, const char *argv[]) {

    if (argc > 1) {
        printf("Este programa não utiliza argumentos na sua invocação.\n");
        return -1;
    }

    setlocale(LC_ALL, "Portuguese");
    size_t number_of_read_bytes;
    char *buffer = (char *) malloc(bytes_to_read * sizeof(char));
    char **argvMA = (char **) malloc((bytes_to_read / 2) * sizeof(char *));
    int i;
    file_open(&g_pFdbStrings, NOME_FICHEIRO_STRINGS, 1);
    file_open(&g_pFdbArtigos, NOME_FICHEIRO_ARTIGOS, 1);

    inicializar_ficheiro_artigos();

    int argCount = 0;
    while ((number_of_read_bytes = read(0, buffer, bytes_to_read)) > 0) {
        i = 0;
        argvMA[i] = strtok(buffer, " ");
        if (strlen(argvMA[i]) == number_of_read_bytes) {
            argvMA[i] = strtok(argvMA[i], "\n");
        }

        while (argvMA[i] != NULL) {
            argvMA[++i] = strtok(NULL, " ");
            argCount++;
        }

        argvMA[i - 1] = strtok(argvMA[i - 1], "\n");

        long codigoArtigo = strtol(argvMA[0], NULL, 10);
        if (argCount == 1) {
            // Vamos apenas mostrar o stock e o preço do artigo
            mostra_info_artigo(codigoArtigo);
        } else if (argCount == 2) {   // Temos de atualizar o stock, e mostrar a nova quantidade
            long acrescento = strtol(argvMA[1], NULL, 10);
            atualiza_mostra_stock(codigoArtigo, acrescento);
        } else {
            // Comando não reconhecido
            return -1;
        }


    }
    file_close(g_pFdbStrings);
    file_close(g_pFdbArtigos);

    return 0;
}