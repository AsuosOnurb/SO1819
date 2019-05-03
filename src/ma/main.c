#include <unistd.h>
#include <stdio.h>
#include <string.h>

#include "../common/fdb.h"
#include "../common/commands.h"
#include "../common/strings.h"
#include "../common/util.h"

int main(int argc, const char *argv[]) {
    // Inicializar os ficheiros necessários
    file_open(&g_pFdbStrings, "STRINGS", 1);

    string_t s1;
    int saveSuccess = string_save("O Linhares é gay", &s1);
    int loadSuccess = string_load(0, &s1);

    printf("Sucesso save: %d, sucesso load: %d\n", saveSuccess, loadSuccess);

    file_close(g_pFdbStrings);
    return 0;
}
