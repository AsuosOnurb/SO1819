#include <stdio.h>
#include "../common/artigo.h"
#include "../common/stock.h"
#include "cv.h"


void mostra_info_stock(long codigoArtigo) {
    artigo_t artigo;

    int errorVal = artigo_load(codigoArtigo, &artigo);
    if (errorVal < 0) {
        printf("Erro: mostra_info_artigo():10 = %d\n", errorVal);
        return;
    }

    // Print do preço
    printf("%f\n", artigo->preco);
    artigo_free(artigo);

    stock_t stock;
    errorVal = stock_load(codigoArtigo, &stock);
    if (errorVal < 0) {
        printf("Erro: mostra_info_artigo():23 = %d\n", errorVal);
        return;
    }

    //Print da quantidade no stock
    printf("%ld\n", stock->quantidade);
    stock_free(stock);

}

int atualiza_mostra_stock(long codigoArtigo, long acrescento) {
    stock_t stock;
    int errorVal = stock_load(codigoArtigo, &stock);
    if (errorVal < 0) {
        printf("Erro: atualiza_mostra_stock():34 = %d\n", errorVal);
        return (-1);
    }

    // Modificar o stock. Obviamente não podemos ter stock negativo.
    if (stock->quantidade + acrescento < 0) {
        // Zeramos o stock.
        stock->quantidade = 0;
        return (-1);
    } else
        stock->quantidade += acrescento;

    errorVal = stock_save(stock);
    if (errorVal < 0) {
        printf("Erro: atualiza_mostra_stock():44 = %d\n", errorVal);
        return (-1);
    }

    printf("%ld\n", stock->quantidade);

    stock_free(stock);

    return 0;
}