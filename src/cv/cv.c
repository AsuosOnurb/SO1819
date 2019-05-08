#include <stdio.h>
#include "cv.h"
#include "../common/artigo.h"
#include "../common/stock.h"

void mostra_info_artigo(long codigoArtigo) {


    artigo_t artigo;

    int errorVal = artigo_load(codigoArtigo, &artigo);
    if (errorVal < 0) {
        printf("Erro: mostra_info_artigo():10 = %d\n", errorVal);
        return;
    }

    printf("Preço do artigo: %f.\n", artigo->preco);
    artigo_free(artigo);

    stock_t stock;
    errorVal = stock_load(codigoArtigo, &stock);
    if (errorVal < 0) {
        printf("Erro: mostra_info_artigo():23 = %d\n", errorVal);
        return;
    }

    printf("Stock disponível: %ld.\n", stock->quantidade);
    stock_free(stock);

}