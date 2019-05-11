#include <stdio.h>
#include "../common/artigo.h"
#include "../common/stock.h"

#include "sv.h"

int mostra_info_artigo(long codigoArtigo, long *quantidade, double *preco) {
    // Carregar o artigo parar consultar o seu preço
    artigo_t artigo;
    if(artigo_load(codigoArtigo, &artigo) != 0)
        return -1;

    // Print do preço
    *preco = artigo->preco;

    // Libertar a memória
    artigo_free(artigo);

    // Carregar o stock do artigo
    stock_t stock;
    if(stock_load(codigoArtigo, &stock) < 0)
        return -2;

    // Print da quantidade no stock
    *quantidade = stock->quantidade;

    // Libertar a memória
    stock_free(stock);

    // Sucesso
    return 0;
}

int atualiza_mostra_stock(long codigoArtigo, long acrescento, long *novoStock) {
    // Carregar o stock
    stock_t stock;
    if(stock_load(codigoArtigo, &stock) != 0)
        return -1;

    // Modificar o stock. Obviamente não podemos ter stock negativo.
    if(stock->quantidade + acrescento < 0) {
        // Zeramos o stock.
        stock->quantidade = 0;
    } else
        stock->quantidade += acrescento;

    // Guardar o stock
    if(stock_save(stock) != 0) {
        return -2;
    }

    // Devolver o resultado
    *novoStock = stock->quantidade;

    // Libertar a memória
    stock_free(stock);

    // Sucesso
    return 0;
}