#include <stdio.h>
#include "../common/artigo.h"
#include "../common/stock.h"

#include "sv.h"
#include "../common/venda.h"

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


    // Escrever a venda em VENDAS se delta < 0
    if (acrescento < 0) {
        // Carregar o artigo para consultar o seu preço
        artigo_t artigo;
        if (artigo_load(codigoArtigo, &artigo) != 0)
            return -3;

        double montanteVenda = artigo->preco * (-1) * acrescento; // acrescento < 0 <=> -acrescento > 0
        long quantidadeVendidos = (-1) * acrescento;

        // Criar nova venda
        venda_t novaVenda = venda_new(codigoArtigo, quantidadeVendidos, montanteVenda);
        // Guardar venda no ficheiro VENDAS
        int errorVal = venda_save(novaVenda);
        if (errorVal < 0) { // Verificar se houve erro
            printf("Algo correu mal: sv.c:75 = %d\n", errorVal);
            return -4;
        }

        // Libertar a memória do artigo e da venda
        artigo_free(artigo);
        venda_free(novaVenda);

    }


    // Sucesso
    return 0;
}

void sv_agrega() {
    // DO IMPORTANT STUFF?
}