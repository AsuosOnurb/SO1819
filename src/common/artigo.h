#ifndef SO1819_ARTIGO_H
#define SO1819_ARTIGO_H


typedef struct artigo {
    char *nome;
    char *codigo;
    int quantidade;
    int preco;
} Artigo;

/**
 * Criação de um artigo novo.
 * @param nomeArtigo Nome do artigo a criar.
 * @param codigoArtigo Código do artigo a criar.
 * @param quantidade Quantidade de artigos a criar.
 * @param preco Preço individual do artigo.
 * @return O artigo criado.
 */
Artigo criaArtigo(char *const nomeArtigo, char *const codigoArtigo, int quantidade, int preco);


#endif //SO1819_ARTIGO_H
