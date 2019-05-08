
#ifndef SO1819_MA_H
#define SO1819_MA_H


#include "../common/artigo.h"

/**
 * Cria um novo artigo, e guarda-o no ficheiro ARTIGOS.
 * @param argv Nome do novo artigo.
 * @param precoArtigo Preço do novo artigo.
 */
artigo_t insere_artigo(char *nomeArtigo, double precoArtigo);

/**
 * Altera o nome de um dado artigo.
 * @param argvMA O comando em argvMA.
 */
void altera_nome(char **argvMA);

/**
 * Altera o preço de um dado artigo.
 * @param argvMA O comando em argvMA.
 * @param novoPreco O novo preço do artigo.
 */
void altera_preco(char **argvMA, double novoPreco);

#endif //SO1819_MA_H
