
#ifndef SO1819_MA_H
#define SO1819_MA_H

#include "../common/fdb.h"
#include "../common/commands.h"
#include "../common/artigo.h"
#include "../common/strings.h"
#include "../common/util.h"

#define bytes_to_read 64

/**
 * @brief Insere um novo artigo no ficheiro ARTIGOS.
 * Este ficheiro contém o código do artigo, o endereço do nome no ficheiro STRINGS e
 * o preço do artigo.<br>
 * Insere, por sua vez, no ficheiro STRINGS, o nome do artigo.
 *
 * @param nomeArtigo
 * @param precoArtigo
 *
 * @return código do artigo.
 */

long insere_artigo(char *nomeArtigo, double precoArtigo);


/**
 * @brief Altera o nome de um artigo.<br>
 * Primeiro acrescenta no ficheiro STRINGS e este devolve o seu endereço,
 * de onde se encontra nesse ficheiro.<br>
 * Por conseguinte, com o código fornecido pelo utilizador,
 * irá ao ficheiro ARTIGOS procurar por esse código e atualizar para o
 * novo endereço de onde se encontra o nome no ficheiro STRINGS.
 *
 * @param argvMA -> Contém o código atribuído ao artigo a alterar e o novo nome.
 *
 * @return 0 em caso de sucesso e >0 em caso de insucesso.
 */

int alteraNome(char** argvMA);

/**
 *@brief Com o código fornecido pelo utilizador, vai ao ficheiro ARTIGOS
 * e procura pela existência desse código. <br>
 * Consequentemente, altera o preço antigo pelo preço novo provido do utilizador.
 *
 * @param argvMA -> Contém o código atribuído ao artigo a alterar e o novo nome.
 *
 * @return 0 em caso de sucesso e >0 em caso de insucesso.
 */

int alteraPreco(char** argvMA);


#endif //SO1819_MA_H
