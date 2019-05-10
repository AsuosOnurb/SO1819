#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include "ma.h"

#include "../common/fdb.h"
#include "../common/commands.h"
#include "../common/artigo.h"
#include "../common/strings.h"
#include "../common/util.h"

#define bytes_to_read 64

#define MAX (1024*1024)

/**
 * Vai inserir no ficheiro ARTIGOS: Código, endereço da posição do nome no ficheiro STRINGS e preço.
 * Vai inserir no ficheiro SRTINGS: Endereço de posição do nome do artigo e o nome do artigo.
 */


long insere_artigo(char *nomeArtigo, double precoArtigo) {
    // Criar string nova
    ssize_t offsetNome = string_save(nomeArtigo);
    if (offsetNome < 0) {
        printf("Algo correu mal: insere_artigo():29 = %ld\n", offsetNome);

    }

    // Criar o artigo propriamente dito
    artigo_t novoArtigo = artigo_new(-1, offsetNome, precoArtigo);

    // Guardar o artigo
    int errorCode = artigo_save(novoArtigo);
    if (errorCode < 0) {
        printf("Algo correu mal: insere_artigo():39 = %d\n", errorCode);
    }

    return novoArtigo->codigo;

}


/**
 * Vai alterar o nome de um dado artigo.
 * Primeiro, acrescenta ao ficheiro STRINGS e atribui-lhe um novo endereco.
 * Segundo, vai atualizar o ficheiro Artigos com o novo endereço do nome do artigo.
 */


int alteraNome(char** argvMA){
    ssize_t offsetName = string_save(argvMA[1]);

    if (offsetName < 0){
        printf("Linha 115. alteraNome()\nErro %ld: %s", offsetName, strerror(errno));
        return 1;
    }

    long codigo = strtol(argvMA[1], NULL, 10);
    artigo_t artigo;
    int offset = artigo_load(codigo, &artigo);

    if (offset < 0){
        printf("Linha 121. alteraNome()\nErro %d: %s", offset, strerror(errno));
        return 2;
    }

    artigo->offsetNome = offsetName;

    int erroVal = artigo_save(artigo);

    if (erroVal < 0){
        printf("Linha 154. alteraPreco()\n Erro %d: %s", erroVal, strerror((errno)));
    }

    artigo_free(artigo);

    return 0;
}


int alteraPreco(char** argvMA){
    long codigo = strtol(argvMA[1], NULL, 10);
    artigo_t artigo;
    int offset = artigo_load(codigo, &artigo);

    if (offset < 0){
        printf("Linha 166. alteraPreco()\n Erro %d: %s", offset, strerror((errno)));
    }

    double precoArtigo = strtod(argvMA[2], NULL);

    artigo->preco = precoArtigo;

    int erroVal = artigo_save(artigo);

    if (erroVal < 0){
        printf("Linha 175. alteraPreco()\n Erro %d: %s", erroVal, strerror((errno)));
    }

    artigo_free(artigo);

    return 0;
}

//void todosCodigos();




