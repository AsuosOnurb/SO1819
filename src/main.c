#include <stdio.h>
#include "servidor_vendas/sv.h"

int main() {
    printf("Hello, World!\n");
    sv_registaVenda(3, "A00001", 45.3);
    return 0;
}