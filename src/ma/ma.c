#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include "ma.h"
#include "../common/artigo.h"

#include "../common/fdb.h"
#include "../common/commands.h"
#include "../common/artigo.h"
#include "../common/strings.h"
#include "../common/util.h"

#define bytes_to_read 64

#define MAX (1024*1024)

artigo_t insere_artigo(char *nomeArtigo, double precoArtigo) {
    // Criar string nova
    ssize_t offsetNome = string_save(nomeArtigo);
    if (offsetNome < 0) {
        printf("Algo correu mal: insere_artigo():59 = %ld\n", offsetNome);

    }

    // Criar o artigo propriamente dito
    artigo_t novoArtigo = artigo_new(-1, offsetNome, precoArtigo);

    // Guardar o artigo
    int errorCode = artigo_save(novoArtigo);
    if (errorCode < 0) {
        printf("Algo correu mal: insere_artigo():68 = %d\n", errorCode);

    }

    return novoArtigo;

}

void altera_nome(char **argvMA) {
    size_t offsetName = string_save(argvMA[1]);

    if (offsetName < 0) {
        printf("Linha 115. alteraNome()\nErro %ld: %s", offsetName, strerror(errno));
    }

    artigo_t artigo;
    int offset = artigo_load(argvMA[1], &artigo);

    if (offset < 0) {
        printf("Linha 121. alteraNome()\nErro %d: %s", offset, strerror(errno));
    }

    artigo->offsetNome = offsetName;

    artigo_save(artigo);

    artigo_free(artigo);

}


void altera_preco(char **argvMA, double novoPreco) {
    artigo_t artigoParaAlterar;

    long codigoArtigo = strtol(argvMA[1], NULL, 10); // char* -> lng

    int errorVal = artigo_load(codigoArtigo, &artigoParaAlterar);

    if (errorVal < 0) {
        printf("Algo correu mal: altera_preco():168 = %d\n", errorVal);
        return;
    }

    artigoParaAlterar->preco = novoPreco;

    // Gravar o artigo depois de o alterar
    errorVal = artigo_save(artigoParaAlterar);

    if (errorVal < 0) {
        printf("Algo correu mal: altera_preco():178 = %d\n", errorVal);
        return;
    }

    artigo_free(artigoParaAlterar);


}

void manutencao_artigos() {
    size_t number_of_read_bytes;

    char *buffer = (char*)malloc(bytes_to_read * sizeof(char));
    int argcMA;
    char **argvMA = (char**)malloc((bytes_to_read/2) * sizeof(char*));
    int fArt = open("ARTIGOS", O_CREAT | O_RDWR | O_TRUNC, 0666);
    int fStr = open("STRINGS", O_CREAT | O_RDWR | O_TRUNC, 0666);
    int i, codigo, endereco;
    codigo = 1;
    endereco = 1;

    //verifica se cria o ficheiro ARTIGOS
    if ((fArt == -1) || (fStr == -1)){
        printf("Erro %d: %s\n", errno, strerror(errno));
        return;
    }

    //verifica se cria o ficheiro SRTINGS
    while ((number_of_read_bytes = read(0, buffer, bytes_to_read)) > 0){
        i = 0;
        argvMA[i] = strtok(buffer," ");
        if (strlen(argvMA[i]) == number_of_read_bytes) {
            argvMA[i] = strtok(argvMA[i], "\n");
        }

        while (argvMA[i] != NULL){
            argvMA[++i] = strtok(NULL, " ");
        }

        argvMA[i-1] = strtok(argvMA[i-1], "\n");

        argcMA = i;

        if(strcmp (argvMA[0],"i") == 0){
            // Inserção de um novo artigo
            char *nomeArtigo = argvMA[1];
            double precoArtigo = strtod(argvMA[2], NULL);
            insere_artigo(nomeArtigo, precoArtigo);

        } else if(strcmp (argvMA[0],"n") == 0){
            // Alteração do nome de um artigo
            altera_nome(argvMA);

        } else if (strcmp (argvMA[0],"p") == 0){
            // Alteração do preço de um artigo
            double novoPreco = strtod(argvMA[2], NULL);
            altera_preco(argvMA, novoPreco);
        }

        codigo++;
        endereco++;
    }
    close(fArt);
    close(fStr);
}
