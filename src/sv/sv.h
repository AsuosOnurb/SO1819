#ifndef SO1819_SV_H
#define SO1819_SV_H

#include <stdio.h>

/**
 * Cria uma entrada no ficheiro VENDAS.txt relativa a uma venda.
 * @param quantidade Quantidade de artigos vendida.
 * @param codigoArtigo Código do artigo vendido.
 * @param precoArtigo Preço individual do artigo.
 * @return O tamanho, em bytes, da string escrita.
 */
size_t regista_venda(int quantidade, char *codigoArtigo, double precoArtigo);

#endif //SO1819_SV_H
