#include <unistd.h>
#include <stdio.h>
#include <string.h>

#include "../common/fdb.h"
#include "../common/commands.h"
#include "../common/strings.h"
#include "../common/util.h"

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include "ma.h"

int main() {
    // Inicializar os ficheiros necessários
    file_open(&g_pFdbStrings, "STRINGS", 1);

    manutencao_artigos();

    file_close(g_pFdbStrings);
    return 0;
}
