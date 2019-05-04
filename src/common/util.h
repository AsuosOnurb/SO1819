#ifndef SO1819_UTIL_H
#define SO1819_UTIL_H

#include "fdb.h"

/**
 * @brief Abre o ficheiro especificado para leitura (e escrita, se especificado)
 *
 * @param fdbuffer O buffer associado ao ficheiro
 * @param file O ficheiro para abrir
 * @param write 1 se quisermos abrir o ficheiro para escrita, assim como leitura
 * @return 0 se tudo correu bem, <0 em caso de erro
 */
int file_open(fdb_t *fdbuffer, char *file, int write);

/**
 * @brief Fecha o ficheiro, buffer e file descriptor indicados.
 *
 * @return fdbuffefr O ficheiro para fechar
 */
int file_close(fdb_t fdbuffer);

#endif //SO1819_UTIL_H
