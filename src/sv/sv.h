

#ifndef SO1819_SV_H
#define SO1819_SV_H

#endif //SO1819_SV_H

/**
 * Cria uma entrada no ficheiro VENDAS.txt relativa a uma venda.
 * @param quantidade Quantidade de artigos vendida.
 * @param codigoArtigo Código do artigo vendido.
 * @param precoArtigo Preço individual do artigo.
 */
ssize_t registaVenda(int quantidade, char *codigoArtigo, double precoArtigo);