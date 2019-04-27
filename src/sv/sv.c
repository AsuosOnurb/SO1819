

#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <zconf.h>
#include "sv.h"

ssize_t registaVenda(int quantidade, char *codigoArtigo, double precoArtigo) {
    // Abrimos o ficheiro de vendas.
    // Criamo-lo caso ainda não exista.
    int fd = open("VENDAS.txt", O_CREAT | O_WRONLY, S_IWUSR);


    //COnversão da quantidade para uma string
    int tamStrQuantidade = snprintf(NULL, 0, "%d", quantidade);
    char *strQuantidade = malloc(tamStrQuantidade + 1); // +1 Pelo \0
    snprintf(strQuantidade, tamStrQuantidade + 1, "%d", quantidade);


    // Conversão do montante para uma string
    double montante = quantidade * precoArtigo;
    int tamStrMontante = snprintf(NULL, 0, "%f", montante);
    char *strMontante = malloc(tamStrMontante + 1);
    snprintf(strMontante, tamStrMontante + 1, "%f", montante);


    // Concatenação das strings para formar a entrada
    char entrada[128] = "";
    snprintf(entrada, sizeof(entrada), "%s %s %s", codigoArtigo, strQuantidade, strMontante);

    int tamanhoEntrada = sizeof(codigoArtigo) - 1 + sizeof(strQuantidade) - 1 + sizeof(strMontante) - 1;

    printf("%s\n", entrada);

    free(strQuantidade);
    free(strMontante);

    // Escrita da entrada no ficheiro VENDAS.txt
    ssize_t bytesEscritos = write(fd, entrada, tamanhoEntrada);
    close(fd);

    return bytesEscritos;


}

