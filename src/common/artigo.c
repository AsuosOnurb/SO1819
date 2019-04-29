
#include <string.h>
#include <stdlib.h>
#include <zconf.h>
#include "artigo.h"

Artigo criaArtigo(char *const nomeArtigo, char *const codigoArtigo, int quantidade, int preco) {

    char *nomeA = strdup(nomeArtigo);
    char *codigoA = strdup(codigoArtigo);

    return (Artigo) {
            .nome = nomeA,
            .codigo = codigoA,
            .quantidade = quantidade,
            .preco = preco
    };
}

Artigo copiaArtigo(Artigo *artigo) {
    return criaArtigo(artigo->nome, artigo->codigo, artigo->quantidade, artigo->preco);
}