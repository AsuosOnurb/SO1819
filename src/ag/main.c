#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>

#include "../common/artigo.h"
#include "../common/util.h"
#include "../common/venda.h"

/**
 * @brief Função principal de execução do agregador.
 *
 * @return 0 se tudo correu bem, !=0 em caso de erro
 */
int main() {
    // Consultar o número de artigos disponíveis para venda
    inicializar_ficheiro_artigos();

    // Inicializar uma array gigante que pode ter todos os artigos lá dentro
    venda_t *vendasAgregadas = (venda_t *) malloc(sizeof(venda_t) * (g_iProximoCodigoUtilizavel));
    memset(vendasAgregadas, '\0', sizeof(venda_t) * (g_iProximoCodigoUtilizavel));

    file_close(g_pFdbArtigos);

    // Fazer com que as vendas sejam lidas a partir do stdin
    fdb_t fdbStdin, fdbStdout, fdbStderr;
    fdb_create(&fdbStdin, STDIN_FILENO);
    fdb_create(&fdbStdout, STDOUT_FILENO);
    fdb_create(&fdbStderr, STDERR_FILENO);

    // Deve chegar 1024 bytes por linha, mas se não chegar, também se resolve
    ssize_t bufferSize = sizeof(char) * 1024;
    ssize_t start = 0;
    char *line = (char *) malloc(bufferSize * sizeof(char));
    while(true) {
        ssize_t bytesRead = fdb_readln(fdbStdin, line + start, bufferSize - start);

        if(bytesRead == 0 && fdbStdin->eof)
            break;

        if(bytesRead < 0) {
            break;
        }

        if(line[bytesRead - 2] != '\n') {
            start = bufferSize;
            bufferSize *= 2;
            line = (char *) realloc(line, bufferSize * sizeof(char));
            continue;
        }

        start = 0;

        venda_t venda = venda_new(-1, -1, -1);
        sscanf(line, "%ld %ld %lf\n", &venda->codigo, &venda->quantidade, &venda->montante);

        if(venda->codigo == -1) {
            venda_free(venda);
            break;
        }

        if(vendasAgregadas[venda->codigo] == NULL) {
            vendasAgregadas[venda->codigo] = venda;
        } else {
            venda_t vendaAtual = vendasAgregadas[venda->codigo];
            vendaAtual->quantidade += venda->quantidade;
            vendaAtual->montante += venda->montante;
            venda_free(venda);
        }
    }

    free(line);

    for(long codigo = 0; codigo < g_iProximoCodigoUtilizavel; codigo++) {
        venda_t venda = vendasAgregadas[codigo];
        fdb_printf(fdbStdout, "%ld %ld %lf\n", venda->codigo, venda->quantidade, venda->montante);
        venda_free(venda);
    }

    fdb_fclose(fdbStdin);
    fdb_fclose(fdbStdout);
    fdb_fclose(fdbStderr);

    return 0;
}
