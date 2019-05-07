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
    ssize_t saveSuccess = string_save("O Ricardo é gay");
    int loadSuccess = string_load(saveSuccess, &s1);

    printf("Sucesso string save: %ld (offset), sucesso load: %d\n", saveSuccess, loadSuccess);

    file_close(g_pFdbStrings);

    // file_open(&g_pFdbArtigos, "ARTIGOS", 1);
    inicializar_ficheiro_artigos();

    artigo_t artigo; // = artigo_new(-1, saveSuccess, 1.0f);

//     int artSaveSuccess = artigo_save(artigo);
//   ssize_t offsetArtigoSaved = artigo->offset;
    int artLoadSuccess = artigo_load(g_iProximoCodigoUtilizavel - 1, &artigo);

//    printf("Sucesso artigo save: %d, sucesso load: %d\n", artSaveSuccess, artLoadSuccess);
//    printf("Artigo saved offset: %ld\n", offsetArtigoSaved);

    file_close(g_pFdbArtigos);

    return 0;
}
