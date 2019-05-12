#ifndef SO1819_SV_H
#define SO1819_SV_H

#define bytes_to_read 64


/**
 * @brief Obtém o stock e o preço de um artigo a partir do ficheiro.
 *
 * @param codigoArtigo O código do artigo cuja informação pretendemos mostrar
 * @param quantidade Onde guardar a quantidade do artigo
 * @param preco Onde guardar o preço do artigo
 *
 * @return 0 se correr tudo bem; -1 em caso de erro
 */
int mostra_info_artigo(long codigoArtigo, long *quantidade, double *preco);

/**
 * @brief Atualiza o stock de um artigo, e faz print do stock atualizado.
 *
 * @param codigoArtigo O código do artigo cujo stock é para ser atualizado.
 * @param acrescento A quantidade a acrescentar ao stock. (Pode ser negativa positiva).
 *
 * @return 0 se tudo correr bem; -1 em caso de erro
 */
int atualiza_mostra_stock(long codigoArtigo, long acrescento, long *novoStock);

/**
 * Começa o processo de agregação.
 * Faz a agregação desde a última vez até ao momento em que este método foi invocado.
 */
void sv_agrega();

#endif //SO1819_SV_H
