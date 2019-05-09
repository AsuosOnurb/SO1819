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
    char *buffer = (char *) malloc(bytes_to_read * sizeof(char));
    char **argArray = (char **) malloc((bytes_to_read / 2) * sizeof(char *));

    file_open(&g_pFdbStrings, "STRINGS", 1);
    file_open(&g_pFdbArtigos, "ARTIGOS", 1);

    inicializar_ficheiro_artigos();

    //int argCount = 0;
    while ((read(0, buffer, bytes_to_read)) > 0) {

        int argCount = 0;

        char *token = strtok(buffer, " ");
        argArray[argCount] = token;
        while (token != NULL) {
            token = strtok(NULL, " ");
            argArray[++argCount] = token;
        }

        long codigoArtigo = strtol(argArray[0], NULL, 10); // char* -> long

        if (argCount == 1) {
            // Vamos apenas mostrar o stock e o preço do artigo
            mostra_info_stock(codigoArtigo);
        } else if (argCount == 2) {   // Temos de atualizar o stock, e mostrar a nova quantidade
            long acrescento = strtol(argArray[1], NULL, 10);
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