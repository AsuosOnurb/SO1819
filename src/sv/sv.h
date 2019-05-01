#ifndef SO1819_SV_H
#define SO1819_SV_H

#include <stdio.h>



/**
 * @brief Regista um aumento no stock do artigo.
 * @param novaQuantidade A nova quantidade de artigos.
 * @param codigoArtigo O código do artigo.
 * @return Código de terminação.

int regista_novo_stock(int novaQuantidade, char* codigoArtigo);
*/

/**
 * @brief Cria uma entrada no ficheiro VENDAS.txt relativa a uma venda.
 * @param quantidade Quantidade de artigos vendida.
 * @param codigoArtigo Código do artigo vendido.
 * @param precoArtigo Preço individual do artigo.
 * @return Código de terminação.
 */
int regista_nova_venda(int quantidade, char *codigoArtigo, double precoArtigo);


/**
 * @brief Devolve a quantidade de artigos que existem em stock, com este código.
 * @param codigoArtigo O código do artigo em questão.
 * @return A quantidade disponível em stock. Devolve -1 caso não exista um artigo com o código especificado.
 */
int obtem_stock(char *codigoArtigo);

/**
 * @brief Devolve o preço do artigo.
 * @param codigoArtigo O código do artigo em questão.
 * @return O preço do artigo. Devolve -1 caso não exista um artigo com o código especificado.
 */
int obtem_preco(char *codigoArtigo);



#endif //SO1819_SV_H
