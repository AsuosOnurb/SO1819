#include <stddef.h>
#include <locale.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#include "../common/strings.h"
#include "../common/util.h"
#include "../common/artigo.h"
#include "../common/sv_protocol.h"
#include "../ma/ma.h"

#include "cv.h"

void client_shutdown();

fdb_t fdbStdin, fdbStdout, fdbStderr;

void client_startup() {
    // Inicializar file descriptor buffers para o stdin e stdout e stderr, pouco provável de falhar
    fdb_create(&fdbStdin, STDIN_FILENO);
    fdb_create(&fdbStdout, STDOUT_FILENO);
    fdb_create(&fdbStderr, STDERR_FILENO);
}


int main() {
    client_startup();

    setlocale(LC_ALL, "Portuguese");
    char *buffer = (char *) malloc(bytes_to_read * sizeof(char));
    char **argArray = (char **) malloc((bytes_to_read / 2) * sizeof(char *));

    // CV doesn't need to read or write from and to "STRINGS" nor to "ARTIGOS"
    // file_open(&g_pFdbStrings, "STRINGS", true, false);
    // file_open(&g_pFdbArtigos, "ARTIGOS", true, false);

    // inicializar_ficheiro_artigos();

    // Get input from user
    // while ((read(0, buffer, bytes_to_read)) > 0) {
    while(fdb_readln(fdbStdin, buffer, bytes_to_read) > 0 && fdbStdin->eof == false) {

        // Split user input into an array of strings
        int argCount = 0;

        char *token = strtok(buffer, " ");
        argArray[argCount] = token;
        while (token != NULL) {
            token = strtok(NULL, " ");
            argArray[++argCount] = token;
        }

        if(argCount <= 0)
            continue;

        long codigoArtigo = strtol(argArray[0], NULL, 10); // char* -> long

        if (argCount == 1) {
            // Ask SV for the available stock quantity and corresponding price
            long quantidade;
            double preco;
            int errorVal = sv_get_info_artigo(codigoArtigo, &quantidade, &preco);

            // Check for errors
            if (errorVal < 0) {
                fdb_printf(fdbStderr, "Algo correu mal: cv/main.c:69 = %d\n", errorVal);
                return errorVal;
            }

            // Print the info
            fdb_printf(fdbStdout, "%ld\n", quantidade);
            fdb_printf(fdbStdout, "%f\n", preco);

        } else if (argCount == 2) {
            // Ask SV to update the stock quantity
            long acrescento = strtol(argArray[1], NULL, 10);
            long novaQuantidadeStock;
            int errorVal = sv_update_mostra_stock(codigoArtigo, acrescento, &novaQuantidadeStock);

            // Check for errors
            if (errorVal < 0) {
                fdb_printf(fdbStderr, "Algo correu mal: cv/main.c:85 = %d\n", errorVal);
                return errorVal;
            }

            // Print the updated stock
            fdb_printf(fdbStdout, "%ld\n", novaQuantidadeStock);


        } else {
            // Unrecognized command
            return -1;
        }

        // memset(buffer, '\0', bytes_to_read);// Prevenir o readln() de retornar imediatamente sem ler!
    }

    // file_close(g_pFdbStrings);
    // file_close(g_pFdbArtigos);

    free(buffer);
    free(argArray);
    client_shutdown();

    return 0;
}

void client_shutdown() {
    file_close(g_pFdbServerFifo);

    fdb_destroy(fdbStdin);
    fdb_destroy(fdbStdout);
    fdb_destroy(fdbStderr);

    _exit(0);
}
