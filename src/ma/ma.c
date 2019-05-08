#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include "ma.h"



#define bytes_to_read 64

#define MAX (1024*1024)

/**
 * Vai inserir no ficheiro ARTIGOS: Código, endereço da posição do nome no ficheiro STRINGS e preço.
 * Vai inserir no ficheiro SRTINGS: Endereço de posição do nome do artigo e o nome do artigo.
 */


/*
void insereArtigos(int fArt, int fStr, int argcMA, char* argvMA[], size_t number_of_read_bytes, int codigo, int endereco){

    char *str = (char*)malloc(number_of_read_bytes*sizeof(char));
    char *art = (char*)malloc(number_of_read_bytes*sizeof(char));
    int bytes_to_write_str, bytes_to_write_art;

    // formata tudo para uma string
    sprintf(str, "%d %s\n", endereco, argvMA[1]);
    sprintf(art, "%d %d %s\n", codigo, endereco, argvMA[2]);

    bytes_to_write_str = strlen(str);
    bytes_to_write_art = strlen(art);

     // escreva para o ficheiro STRINGS

    size_t strSize = write(fStr, str, bytes_to_write_str);
    

     // escreve para o ficheiro ARTIGOS
    size_t artSize = write(fArt, art, bytes_to_write_art);


     // Verifica se os ficheiros foram escritos ou não;
     // Se o resultado da escrita for -1 deu erro na escrita e é exibido o erro
     // Se o resultado da escrita for 0, quer dizer que o ficheiro não foi escrito

    if (strSize == -1 || artSize == -1){
        printf("Erro %d: %s", errno, strerror(errno));
    } else if(strSize == 0 || artSize == 0){
        printf("Erro %d: %s", errno, strerror(errno));
    }

}
 */

long insere_artigo(char *nomeArtigo, double precoArtigo) {
    // Criar string nova
    ssize_t offsetNome = string_save(nomeArtigo);
    if (offsetNome < 0) {
        printf("Algo correu mal: insere_artigo():59 = %ld\n", offsetNome);

    }

    // Criar o artigo propriamente dito
    artigo_t novoArtigo = artigo_new(-1, offsetNome, precoArtigo);

    // Guardar o artigo
    int errorCode = artigo_save(novoArtigo);
    if (errorCode < 0) {
        printf("Algo correu mal: insere_artigo():68 = %d\n", errorCode);
    }

    return novoArtigo->codigo;

}


/**
 * Vai alterar o nome de um dado artigo.
 * Primeiro, acrescenta ao ficheiro STRINGS e atribui-lhe um novo endereco.
 * Segundo, vai atualizar o ficheiro Artigos com o novo endereço do nome do artigo.
 */
 /*
void alteraNome(int fArt,int fStr, int argcMA, char* argvMA[], int codigo, int endereco, size_t number_of_read_bytes){
    char *str = (char*)malloc(number_of_read_bytes*sizeof(char));
    char *art = (char*)malloc(number_of_read_bytes*sizeof(char));
    char *tmp = (char*)malloc(bytes_to_read*sizeof(char));
    char *art_new = (char*)malloc(bytes_to_read*sizeof(char));
    char *buffer = (char*)malloc(number_of_read_bytes*sizeof(char));
    int bytes_to_write;
    size_t number_of_bytes;
    int i = 0;
    close(fArt);

    int fart = open("ARTIGOS", O_RDONLY, 0666);
    if ((fart == -1)){
        printf("Erro %d: %s\n", errno, strerror(errno));
        //return errno;
    }

     //Formata tudo para uma string

    sprintf(str, "%d %s\n", endereco, argvMA[2]);
    bytes_to_write = strlen(str);


     //Acrescenta o nome no ficheiro de strings e,
     //é atribuído um novo endereco ao nome do artigo

    size_t strSize = write(fStr, str, bytes_to_write);

    

     // Vai procurar pelo código do nome a alterar.

    while((number_of_bytes = read(fart, buffer, bytes_to_read)) > 0){
        if (buffer[0] == atoi(argvMA[1])){
            tmp = buffer;
        }
    }


     // Adiciona o novo endereço à linha a alterar

    sprintf(art_new, "%d %d %d", atoi(argvMA[1]), endereco, tmp[2]);
    printf("%s\n", art_new);

    close(fArt);


}
*/

int alteraNome(char** argvMA){
    ssize_t offsetName = string_save(argvMA[1]);

    if (offsetName < 0){
        printf("Linha 115. alteraNome()\nErro %ld: %s", offsetName, strerror(errno));
        return 1;
    }

    long codigo = strtol(argvMA[1], NULL, 10);
    artigo_t artigo;
    int offset = artigo_load(codigo, &artigo);

    if (offset < 0){
        printf("Linha 121. alteraNome()\nErro %d: %s", offset, strerror(errno));
        return 2;
    }

    artigo->offsetNome = offsetName;

    int erroVal = artigo_save(artigo);

    if (erroVal < 0){
        printf("Linha 154. alteraPreco()\n Erro %d: %s", erroVal, strerror((errno)));
    }

    artigo_free(artigo);

    return 0;
}


int alteraPreco(char** argvMA){
    long codigo = strtol(argvMA[1], NULL, 10);
    artigo_t artigo;
    int offset = artigo_load(codigo, &artigo);

    if (offset < 0){
        printf("Linha 166. alteraPreco()\n Erro %d: %s", offset, strerror((errno)));
    }

    double precoArtigo = strtod(argvMA[2], NULL);

    artigo->preco = precoArtigo;

    int erroVal = artigo_save(artigo);

    if (erroVal < 0){
        printf("Linha 175. alteraPreco()\n Erro %d: %s", erroVal, strerror((errno)));
    }

    artigo_free(artigo);

    return 0;
}

//void todosCodigos();




