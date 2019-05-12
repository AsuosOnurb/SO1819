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
#include "../common/sv_protocol.h"

#define bytes_to_read 64

#define MAX (1024*1024)

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

    long codigo = novoArtigo->codigo;
    artigo_free(novoArtigo);

    return codigo;
}

int alteraNome(char** argvMA){
    ssize_t offsetName = string_save(argvMA[1]);

    if (offsetName < 0){
        printf("Linha 115. alteraNome()\nErro %ld: %s", offsetName, strerror(errno));
        return -1;
    }

    long codigo = strtol(argvMA[1], NULL, 10);
    artigo_t artigo;


    int offset = artigo_load(codigo, &artigo);

    if (offset < 0){
        printf("Linha 121. alteraNome()\nErro %d: %s", offset, strerror(errno));
        return -2;
    }

    string_mark(artigo->offsetNome);
    
    artigo->offsetNome = offsetName;

    int erroVal = artigo_save(artigo);

    if (erroVal < 0){
        printf("Linha 154. alteraPreco()\n Erro %d: %s", erroVal, strerror((errno)));
    }

    artigo_free(artigo);

    // Informar o servidor da alteração de nome
    sv_send_instruction(SV_INSTRUCTION_INVALIDAR_CACHE_ARTIGOS, (const char *) &codigo, sizeof(codigo), getpid());

    return 0;
}


int alteraPreco(char** argvMA){
    long codigo = strtol(argvMA[1], NULL, 10);

    artigo_t artigo;
    int offset = artigo_load(codigo, &artigo);

    if (offset < 0){
        printf("Linha 166. alteraPreco()\n Erro %d: %s", offset, strerror((errno)));
        return -1;
    }

    double precoArtigo = strtod(argvMA[2], NULL);

    artigo->preco = precoArtigo;

    int erroVal = artigo_save(artigo);

    if (erroVal < 0){
        printf("Linha 175. alteraPreco()\n Erro %d: %s", erroVal, strerror((errno)));
    }

    artigo_free(artigo);

    // Informar o servidor da alteração de preço
    sv_send_instruction(SV_INSTRUCTION_INVALIDAR_CACHE_ARTIGOS, (const char *) &codigo, sizeof(codigo), getpid());

    return 0;
}
