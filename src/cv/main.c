#include <stddef.h>
#include <locale.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#include "../common/strings.h"
#include "../common/util.h"
#include "../common/artigo.h"
#include "../common/sv_backend.h"
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

    // CV doesn't need to read or write from and to "STRINGS" nor to "ARTIGOS"
    // file_open(&g_pFdbStrings, "STRINGS", true, false);
    // file_open(&g_pFdbArtigos, "ARTIGOS", true, false);

    inicializar_ficheiro_artigos();

    // Get input from user
    while ((read(0, buffer, bytes_to_read)) > 0) {

        // Split user input into an array of strings
        int argCount = 0;

        char *token = strtok(buffer, " ");
        argArray[argCount] = token;
        while (token != NULL) {
            token = strtok(NULL, " ");
            argArray[++argCount] = token;
        }


        long codigoArtigo = strtol(argArray[0], NULL, 10); // char* -> long

        if (argCount == 1) {
            // Ask SV for the available stock quantity and corresponding price
            long quantidade;
            double preco;
            int errorVal = sv_get_info_artigo(codigoArtigo, &quantidade, &preco);

            // Check for errors
            if (errorVal < 0) {
                printf("Algo correu mal: cv/main.c():60 = %d\n", errorVal);
                return errorVal;
            }

            // Print the info
            printf("%ld\n", quantidade);
            printf("%f\n", preco);

        } else if (argCount == 2) {
            // Ask SV to update the stock quantity
            long acrescento = strtol(argArray[1], NULL, 10);
            long novaQuantidadeStock;
            int errorVal = sv_update_mostra_stock(codigoArtigo, acrescento, &novaQuantidadeStock);

            // Check for errors
            if (errorVal < 0) {
                printf("Algo correu mal: cv/main.c():75 = %d\n", errorVal);
                return errorVal;
            }

            // Print the updated stock
            printf("%ld\n", novaQuantidadeStock);


        } else {
            // Unrecognized command
            return -1;
        }

    }

    // file_close(g_pFdbStrings);
    // file_close(g_pFdbArtigos);

    return 0;
}