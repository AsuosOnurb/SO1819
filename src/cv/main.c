#include <stddef.h>
#include <locale.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../common/strings.h"
#include "../common/util.h"
#include "cv.h"
#include "../common/artigo.h"


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
    file_open(&g_pFdbStrings, "STRINGS", 1);
    file_open(&g_pFdbArtigos, "ARTIGOS", 1);

    inicializar_ficheiro_artigos();

    /*
    printf("i <nome> <preço> -> Insere novo artigo, mostra o código\n");
    printf("n <código> <preço> -> altera nome do artigo\n");
    printf("p <código> <preço> -> altera o preço do artigo\n");
    printf("a -> mostra todos os códigos e os respetivos artigos\n");
    */
    //verifica se cria o ficheiro SRTINGS
    while ((number_of_read_bytes = read(0, buffer, bytes_to_read)) > 0) {
        i = 0;
        argvMA[i] = strtok(buffer, " ");
        if (strlen(argvMA[i]) == number_of_read_bytes) {
            argvMA[i] = strtok(argvMA[i], "\n");
        }

        while (argvMA[i] != NULL) {
            argvMA[++i] = strtok(NULL, " ");
        }

        argvMA[i - 1] = strtok(argvMA[i - 1], "\n");

        if(strcmp (argvMA[0],"i") == 0){
            long codigo;
            char *nomeArtigo = argvMA[1];
            double precoArtigo;
            sscanf(argvMA[2], "%lf", &precoArtigo); // Conversão da string para double

            codigo = insere_artigo(nomeArtigo, precoArtigo);
            printf("Código do artigo: %ld\n", codigo);

        } else if(strcmp (argvMA[0],"n") == 0){
            int resultado;
            resultado = alteraNome(argvMA);
            if (resultado == 0) {
                printf("O nome do seu artigo foi alterado com sucesso\n");
            } else {
                printf("O nome do seu artigo não foi alterado. Tente novamente!\nErro %d: erro no alteraNome()", resultado);
            }
        } else if (strcmp (argvMA[0],"p") == 0){
            int resultado;
            resultado = alteraPreco(argvMA);
            if (resultado == 0) {
                printf("O nome do seu artigo foi alterado com sucesso\n");
            } else {
                printf("O nome do seu artigo não foi alterado. Tente novamente!\nErro %d: erro no alteraPreco()", resultado);
            }
        }

        return 0;
    }
}