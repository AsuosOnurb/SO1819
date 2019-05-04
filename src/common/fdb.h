#ifndef FDB_H
#define FDB_H

#include <sys/types.h>
#include <stdbool.h>

/**
 * @brief Define um "file descriptor buffer" (leia-se fdbuffer) com um descritor de ficheiro associado, na mesma ideia do que a estrutura FILE do C.
 * Esta estrutura tem capacidade de buffering em memória, para evitar fazer reads desnecessários, e deste modo aumentar a performance.
 * Esta estrutura tem também a capacidade de ler e escrever dados de vários tipos de e para o descritor de ficheiros associado.
 */
typedef struct fdb {
    /** @brief O índice do descritor de ficheiros associado com este fdbuffer. */
    int fd;
    /** @brief Tamanho do buffer de leitura. */
    size_t size;
    /** @brief Posição de início do buffer de leitura. */
    size_t start;
    /** @brief Ocupação do buffer. */
    size_t occupation;
    /** @brief Bytes já lidos do descritor de ficheiros. */
    char *buffer;
    /** @brief Verdadeiro se atingimos EOF. */
    bool eof;
} *fdb_t;

/**
 * @brief Cria um fdbuffer com o descritor indicado.
 * 
 * @param fd O descritor de ficheiros a associar ao fdbuffer
 * @param fdbuf Onde guardar o fdbuffer criado
 * 
 * @return 0 se correu tudo bem, <0 em caso de erro
 */
int fdb_create(int fd, fdb_t *fdbuf);

/**
 * @brief Destrói um fdbuffer.
 * 
 * @param fdbuf O fdbuffer a destruir
 * 
 * @return 0 se correu tudo bem, <0 em caso de erro
 */
int fdb_destroy(fdb_t fdbuf);

/**
 * @brief Le um caracter a partir do descritor de ficheiros associado ao fdbuffer especificado.
 * 
 * @param fdbuf O fdbuffer a partir do qual ler
 * 
 * @return Se tudo correu bem, o caracter lido a partir do descritor; em caso de erro, retorna um número negativo, como código de erro
 */
char fdb_readc(fdb_t fdbuf);

/**
 * @brief Lê o número de bytes especificado a partir do descritor de ficheiros associado ao fdbuffer especificado.
 * Bloqueia dependendo da flag de bloqueio do descritor de ficheiro associado.
 * Pode ler menos bytes do que {@param size} indica; é intencional, pois podemos ter chegado a EOF.
 *
 * @param fdbuf O fdbuffer a partir do qual ler
 * @param buf O buffer onde guardar os dados lidos
 * @param size O tamanho máximo do buffer especificado em {@param buf}
 *
 * @return O número de bytes efetivamente lidos, ou <0 em caso de erro
 */
ssize_t fdb_read(fdb_t fdbuf, void *buf, size_t size);

/**
 * @brief Lẽ uma string a partir do descritor associado.
 * Bloqueia dependendo da flag de bloqueio do descritor de ficheiro associado.
 * Pode ler menos bytes do que {@param size} indica; é intencional, pois podemos ter chegado a EOF, ou a string pode ser menor do que o parâmetro {@param size}.
 *
 * @param fdbuf O fdbuffer a partir do qual ler
 * @param buf O buffer onde guardar os dados lidos
 * @param size O tamanho máximo do buffer especificado em {@param buf}
 *
 * @return O número de bytes efetivamente lidos, ou <0 em caso de erro
 */
ssize_t fdb_reads(fdb_t fdbuf, char *buf, size_t size);

/**
 * @brief Lẽ uma linha (que termina com \n) a partir do descritor associado.
 * Bloqueia dependendo da flag de bloqueio do descritor de ficheiro associado.
 * Pode ler menos bytes do que {@param size} indica; é intencional, pois podemos ter chegado a EOF, ou a string pode ser menor do que o parâmetro {@param size}.
 *
 * @param fdbuf O fdbuffer a partir do qual ler
 * @param buf O buffer onde guardar a linha lida. Inclúi, na posição (return - 2), o caracter '\n', para que seja possível, desse modo, distinguir se a linha lida foi completa, ou se o buffer {@param buf} não tinha tamanho suficiente para ler uma linha completa.
 * @param size O tamanho máximo do buffer especificado em {@param buf}
 *
 * @return O número de bytes efetivamente lidos, ou <0 em caso de erro
 */
ssize_t fdb_readln(fdb_t fdbuf, char *buf, size_t size);

/**
 * @brief Escreve para o descritor de ficheiros os dados especificados.
 * Bloqueia dependendo da flag de bloqueio do descritor de ficheiro associado.
 *
 * @param fdbuf O fdbuffer para o qual escrever
 * @param buf O buffer que contém os dados a escrever
 * @param size O número de bytes de {@param buf} a escrever
 *
 * @return 0 em caso de sucesso, ou <0 em caso de erro
 */
int fdb_write(fdb_t fdbuf, const void *buf, size_t size);

/**
 * @brief Escreve para o descritor de ficheiros uma string formatada no mesmo estilo da função printf().
 * Bloqueia dependendo da flag de bloqueio do descritor de ficheiro associado.
 *
 * @param fdbuf O fdbuffer para o qual escrever
 * @param fmt O formato da string a escrever, no mesmo formato da família de funções printf()
 * @param ... As variáveis a substituir no formato especificado
 *
 * @return 0 em caso de sucesso, ou <0 em caso de erro
 */
int fdb_printf(fdb_t fdbuf, const char *fmt, ...);

/**
 * @brief Abre o ficheiro especificado, e cria um fdbuffer associado ao descritor de ficheiros retornado.
 * 
 * @param fdbuf Onde guardar o fdbuffer criado
 * @param path O caminho para o ficheiro a abrir
 * @param flags Bitwised-flags sobre como abrir o ficheiro (que irão ser passadas à system call read)
 * @param mode Modo de permissões para o ficheiro a abrir
 *
 * @return <0 on error, or 0 on success
 */
int fdb_fopen(fdb_t *fdbuf, const char *path, int flags, mode_t mode);

/**
 * @brief Fecha o fdbuffer e o file descriptor associado com o fdbuffer especificado.
 *
 * @param fdbuf O fdbuffer para fechar e destruir
 * 
 * @return 0 em caso de sucesso, <0 em caso de erro
 */
int fdb_fclose(fdb_t fdbuf);

/**
 * @brief Faz seek do descritor de ficheiros até à posição especificada.
 *
 * @param fdbuf O fdbuffer para fazer seek
 * @param offset O offset para o qual fazer seek
 * @param seekFlags Flags para passar à chamada lseek
 *
 * @return O novo offset para leitura/escrita em caso de sucesso, <0 em caso de erro
 */
int fdb_lseek(fdb_t fdbuf, off_t offset, int seekFlags);

#endif // FDB_H
