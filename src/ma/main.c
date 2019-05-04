#include <unistd.h>
#include <stdio.h>
#include <string.h>

#include "../common/fdb.h"
#include "../common/commands.h"
#include "../common/artigo.h"
#include "../common/strings.h"
#include "../common/util.h"

int main(int argc, const char *argv[]) {
    // Inicializar os ficheiros necessários
    file_open(&g_pFdbStrings, "STRINGS", 1);

    string_t s1;
    ssize_t saveSuccess = string_save("O Linhares é gay");
    int loadSuccess = string_load(saveSuccess, &s1);

    printf("Sucesso save: %ld, sucesso load: %d\n", saveSuccess, loadSuccess);

    file_close(g_pFdbStrings);

    file_open(&g_pFdbArtigos, "ARTIGOS", 1);

    artigo_t artigo = artigo_new(g_iProximoCodigoUtilizavel++, saveSuccess, 1.0f);

    int artSaveSuccess = artigo_save(artigo);
    int artLoadSuccess = artigo_load(g_iProximoCodigoUtilizavel - 1, &artigo);

    printf("Sucesso artigo save: %d, sucesso load: %d\n", artSaveSuccess, artLoadSuccess);

    file_close(g_pFdbArtigos);

    return 0;
}
