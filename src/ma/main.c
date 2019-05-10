#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <locale.h>
#include <errno.h>

#include "../common/fdb.h"
#include "../common/commands.h"
#include "../common/artigo.h"
#include "../common/strings.h"
#include "../common/util.h"

#include "ma.h"

#define bytes_to_read 64

int main(int argc, const char *argv[]) {
    if (argc > 1){
        printf("Erro %d: %s",errno, strerror(errno));
        return errno;
    }


    setlocale(LC_ALL, "Portuguese");
    size_t number_of_read_bytes;
    char *buffer = (char*)malloc(bytes_to_read * sizeof(char));
    char **argvMA = (char**)malloc((bytes_to_read/2) * sizeof(char*));
    int i;

    file_open(&g_pFdbStrings, "STRINGS", true, true);
    inicializar_ficheiro_artigos();


    //verifica se cria o ficheiro SRTINGS
    while ((number_of_read_bytes = read(0, buffer, bytes_to_read)) > 0){
        i = 0;
        argvMA[i] = strtok(buffer," ");
        if (strlen(argvMA[i]) == number_of_read_bytes) {
            argvMA[i] = strtok(argvMA[i], "\n");
        }

        while (argvMA[i] != NULL){
            argvMA[++i] = strtok(NULL," ");
        }

        argvMA[i-1] = strtok(argvMA[i-1], "\n");



        if(strcmp (argvMA[0],"i") == 0){
            long codigo;
            char *nomeArtigo = argvMA[1];
            double precoArtigo = strtod(argvMA[2], NULL);


            codigo = insere_artigo(nomeArtigo, precoArtigo);
            printf("Código do artigo: %ld\n", codigo);

        } else if(strcmp (argvMA[0],"n") == 0){
            int resultado;
            resultado = alteraNome(argvMA);
            if (resultado < 0) {
                printf("O nome do seu artigo não foi alterado. Tente novamente!\nErro %d: erro no alteraNome()", resultado);
            }
        } else if (strcmp (argvMA[0],"p") == 0){
            int resultado;
            resultado = alteraPreco(argvMA);
            if (resultado < 0) {
                printf("O nome do seu artigo não foi alterado. Tente novamente!\nErro %d: erro no alteraPreco()", resultado);
            }
        }



    }
    file_close(g_pFdbStrings);
    file_close(g_pFdbArtigos);

    return 0;
}
