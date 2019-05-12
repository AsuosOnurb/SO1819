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

#include "ag.h"

int main2();

int main() {
    int retCode = main2();
    // fprintf(stderr, "Ret code: %d\n", retCode);
    return retCode;
}

int main2() {
    // fprintf(stderr, "OLAAAAAAAAAAAA SOU EU\n");
    // Consultar o número de artigos disponíveis para venda
    inicializar_ficheiro_artigos();

    // Inicializar uma array gigante que pode ter todos os artigos lá dentro
    venda_t *vendasAgregadas = (venda_t *) malloc(sizeof(venda_t) * (g_iProximoCodigoUtilizavel - 1));
    memset(vendasAgregadas, '\0', sizeof(venda_t) * (g_iProximoCodigoUtilizavel - 1));

    file_close(g_pFdbArtigos);

    // Fazer com que as vendas sejam lidas a partir do stdin
    // char buff[4096];
    // readlink("/proc/self/fd/0", buff, 4096);
    // fprintf(stderr, "%s\n", buff);

    fdb_t fdbStdin, fdbStdout, fdbStderr;
    fdb_create(&fdbStdin, STDIN_FILENO);
    fdb_create(&fdbStdout, STDOUT_FILENO);
    fdb_create(&fdbStderr, STDERR_FILENO);
    // fprintf(stderr, "Setup properly completed\n");

    // Deve chegar 1024 bytes por linha, mas se não chegar, também se resolve
    ssize_t bufferSize = sizeof(char) * 1024;
    ssize_t start = 0;
    char *line = (char *) malloc(bufferSize * sizeof(char));
    while(true) {
        // fprintf(stderr, "\n");
        ssize_t bytesRead = fdb_readln(fdbStdin, line + start, bufferSize - start);
        // fprintf(stderr, "Depois do readln\n");

        // if(bytesRead == 0 && ((bytesRead = fdb_readln(fdbStdin, line + start, bufferSize - start)) == 0))
        if(bytesRead == 0 && fdbStdin->eof)
            break;

        if(bytesRead < 0) {
            break;
        }
        // fprintf(stderr, "Depois do readln e do <0\n");

        if(line[bytesRead - 2] != '\n') {
            start = bufferSize;
            bufferSize *= 2;
            line = (char *) realloc(line, bufferSize * sizeof(char));
            // fprintf(stderr, "Aumentado size do buffer de linha para %ld\n", bufferSize);
            // fprintf(stderr, "Lida linha até agora: %s\n", line);
            continue;
        }

        start = 0;

        // fprintf(stderr, "Linha lida: %s", line);

        venda_t venda = venda_new(-1, -1, -1);
        sscanf(line, "%ld %ld %lf\n", &venda->codigo, &venda->quantidade, &venda->montante);

        if(venda->codigo == -1) {
            venda_free(venda);
            break;
        }

        if(vendasAgregadas[venda->codigo] == NULL) {
            // fprintf(stderr, "Nova venda! %ld\n", venda->codigo);
            vendasAgregadas[venda->codigo] = venda;
        } else {
            // fprintf(stderr, "Somando valores para %ld\n", venda->codigo);
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
        // fdb_printf(fdbStderr, "%ld %ld %lf\n", venda->codigo, venda->quantidade, venda->montante);
    }

    fdb_fclose(fdbStdin);
    fdb_fclose(fdbStdout);
    fdb_fclose(fdbStderr);

    return 0;
}
