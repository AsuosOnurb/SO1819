#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <locale.h>
#include <errno.h>
#include <unistd.h>

#include "../common/fdb.h"
#include "../common/commands.h"
#include "../common/artigo.h"
#include "../common/strings.h"
#include "../common/util.h"
#include "../common/sv_protocol.h"

#include "ma.h"

#define bytes_to_read 64

/**
 * @brief Função principal do binário de manutenção de artigos.
 *
 * @return 0 se correu tudo bem, !=0 em caso de erro
 */
int main() {
    fdb_t fdbStdin;
    fdb_create(&fdbStdin, STDIN_FILENO);

    setlocale(LC_ALL, "Portuguese");
    size_t number_of_read_bytes;
    char *buffer = (char*)malloc(bytes_to_read * sizeof(char));
    char **argvMA = (char**)malloc((bytes_to_read/2) * sizeof(char*));
    int i;

    inicializar_ficheiro_strings();
    inicializar_ficheiro_artigos();

    //verifica se cria o ficheiro SRTINGS
    // while ((number_of_read_bytes = read(0, buffer, bytes_to_read)) > 0){
    while((number_of_read_bytes = fdb_readln(fdbStdin, buffer, bytes_to_read)) > 0) {
        i = 0;
        argvMA[i] = strtok(buffer," ");
        if (strlen(argvMA[i]) == number_of_read_bytes) {
            argvMA[i] = strtok(argvMA[i], "\n");
        }

        while (argvMA[i] != NULL) {
            argvMA[++i] = strtok(NULL," ");
        }

        argvMA[i-1] = strtok(argvMA[i-1], "\n");


        // Inserção de um artigo
        if(strcmp (argvMA[0],"i") == 0){
            long codigo;
            char *nomeArtigo = argvMA[1];
            double precoArtigo = strtod(argvMA[2], NULL);


            codigo = insere_artigo(nomeArtigo, precoArtigo);
            printf("%ld\n", codigo);
        } else if (strcmp(argvMA[0], "n") == 0) { // Mudar nome d'um artigo
            int resultado;
            resultado = alteraNome(argvMA);
            if (resultado < 0) {
                printf("O nome do seu artigo não foi alterado. Tente novamente!\nErro %d: erro no alteraNome()", resultado);
            }
        } else if (strcmp(argvMA[0], "p") == 0) { // Mudar preço de um artigo
            int resultado;
            resultado = alteraPreco(argvMA);
            if (resultado < 0) {
                printf("O nome do seu artigo não foi alterado. Tente novamente!\nErro %d: erro no alteraPreco()", resultado);
            }
        } else if (strcmp(argvMA[0], "a") == 0) { // Invoccar o agregador
            sv_send_instruction(SV_INSTRUCTION_EXECUTAR_AG, NULL, 0, getpid());
            file_close(g_pFdbServerFifo);
            g_pFdbServerFifo = NULL;
        }

    }

    free(buffer);
    free(argvMA);
    file_close(g_pFdbStrings);
    file_close(g_pFdbArtigos);
    fdb_destroy(fdbStdin);

    return 0;
}
