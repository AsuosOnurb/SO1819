#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <zconf.h>

#include <sys/stat.h>

#include "sv.h"
#include "../common/fdb.h"
#include "../common/artigo.h"
#include "../common/venda.h"

int registar_venda(artigo_t artigo, long quantidade) {
    double montante = quantidade * artigo->preco;

    // fdb_printf(fdbuf, "%s %d %f", codigoArtigo, quantidade, montante);
    venda_t venda = venda_new(artigo->codigo, quantidade, montante);


    fdb_fclose(fdbuf);

    // Retornar que não houve erro
    return 0;
}










