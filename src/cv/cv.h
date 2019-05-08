
#ifndef SO1819_CV_H
#define SO1819_CV_H

/**
 * Mostra no stdout o preço e o stock disponível do artigo com o código especificado.
 * @param codigoArtigo O código do artigo cuja informação pretendemos mostrar.
 */
void mostra_info_artigo(long codigoArtigo);

/**
 * Atualiza o stock de um artigo, e faz print do stock atualizado.
 * @param codigoArtigo O código do artigo cujo stock é para ser atualizado.
 * @param acrescento A quantidade a acrescentar ao stock. (Pode ser negativa positiva).
 * @return 0 se tudo correr bem; -1 em caso de erro.
 */
int atualiza_mostra_stock(long codigoArtigo, long acrescento);

#endif //SO1819_CV_H
