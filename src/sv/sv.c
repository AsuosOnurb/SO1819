#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <zconf.h>

#include <sys/stat.h>

#include "sv.h"
#include "../common/fdb.h"

int regista_nova_venda(int quantidade, char *codigoArtigo, double precoArtigo) {
    // Abrimos o ficheiro de vendas.
    fdb_t fdbuf;
    fdb_fopen(&fdbuf, "VENDAS.txt", O_CREAT | O_WRONLY, S_IWUSR);


    // Conversão do montante para uma string
    double montante = quantidade * precoArtigo;

    fdb_printf(fdbuf, "%s %d %f", codigoArtigo, quantidade, montante);


    fdb_fclose(fdbuf);

    // Retornar que não houve erro
    return 0;
}










