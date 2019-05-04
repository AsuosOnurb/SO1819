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

void insereArtigos(int fArt, int fStr, int argcMA, char* argvMA[], size_t number_of_read_bytes, int codigo, int endereco){
    char *str = (char*)malloc(number_of_read_bytes*sizeof(char));
    char *art = (char*)malloc(number_of_read_bytes*sizeof(char));
    int bytes_to_write_str, bytes_to_write_art;

    /**
     * formata tudo para uma string
     */ 
    sprintf(str, "%d %s\n", endereco, argvMA[1]);
    sprintf(art, "%d %d %s\n", codigo, endereco, argvMA[2]);

    bytes_to_write_str = strlen(str);
    bytes_to_write_art = strlen(art);
    /**
     * escreva para o ficheiro STRINGS
     */
    size_t strSize = write(fStr, str, bytes_to_write_str);
    
    /**
     * escreve para o ficheiro ARTIGOS
     */
    size_t artSize = write(fArt, art, bytes_to_write_art);

    /**
     * Verifica se os ficheiros foram escritos ou não;
     * Se o resultado da escrita for -1 deu erro na escrita e é exibido o erro
     * Se o resultado da escrita for 0, quer dizer que o ficheiro não foi escrito
     */ 
    if (strSize == -1 || artSize == -1){
        printf("Erro %d: %s", errno, strerror(errno));
    } else if(strSize == 0 || artSize == 0){
        printf("Erro %d: %s", errno, strerror(errno));
    }
    

}
/**
 * Vai alterar o nome de um dado artigo.
 * Primeiro, acrescenta ao ficheiro STRINGS e atribui-lhe um novo endereco.
 * Segundo, vai atualizar o ficheiro Artigos com o novo endereço do nome do artigo.
 */ 
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
    /**
     * Formata tudo para uma string
     */ 
    sprintf(str, "%d %s\n", endereco, argvMA[2]);
    bytes_to_write = strlen(str);

    /**
     * Acrescenta o nome no ficheiro de strings e, 
     * é atribuído um novo endereco ao nome do artigo
     */ 
    size_t strSize = write(fStr, str, bytes_to_write);

    
    /**
     * Vai procurar pelo código do nome a alterar.
     */ 
    while((number_of_bytes = read(fart, buffer, bytes_to_read)) > 0){
        if (buffer[0] == atoi(argvMA[1])){
            tmp = buffer;
        }
    }

    /**
     * Adiciona o novo endereço à linha a alterar
     */ 
    sprintf(art_new, "%d %d %d", atoi(argvMA[1]), endereco, tmp[2]);
    printf("%s\n", art_new);

    close(fArt);


}
void alteraPreco(int fArt, int fStr, int argcMA, char* argvMA[], size_t number_of_read_bytes, int codigo, int endereco){
    printf("p\n");
}

void manutencao_artigos() {
    size_t number_of_read_bytes;
    
    char *buffer = (char*)malloc(bytes_to_read * sizeof(char));
    int argcMA;
    char **argvMA = (char**)malloc((bytes_to_read/2) * sizeof(char*));
    int fArt = open("ARTIGOS", O_CREAT | O_RDWR | O_TRUNC, 0666);
    int fStr = open("STRINGS", O_CREAT | O_RDWR | O_TRUNC, 0666);
    int i, codigo, endereco;
    codigo = 1;
    endereco = 1;

    //verifica se cria o ficheiro ARTIGOS
    if ((fArt == -1) || (fStr == -1)){
        printf("Erro %d: %s\n", errno, strerror(errno));
        //return errno;
    }

    //verifica se cria o ficheiro SRTINGS
    while ((number_of_read_bytes = read(0, buffer, bytes_to_read)) > 0){
        i = 0;
        argvMA[i] = strtok(buffer," ");
        if (strlen(argvMA[i]) == number_of_read_bytes) {
            argvMA[i] = strtok(argvMA[i], "\n");
        }

        while (argvMA[i] != NULL){
            argvMA[++i] = strtok(NULL," ");  
        }
        
        argvMA[i-1] = strtok(argvMA[i-1], "\n");
        
        argcMA = i;
        
        if(strcmp (argvMA[0],"i") == 0){
            insereArtigos(fArt, fStr, argcMA,argvMA, number_of_read_bytes, codigo, endereco);
        } else if(strcmp (argvMA[0],"n") == 0){
            alteraNome(fArt, fStr, argcMA,argvMA, codigo, endereco, number_of_read_bytes);
        } else if (strcmp (argvMA[0],"p") == 0){
            alteraPreco(fArt, fStr, argcMA, argvMA, number_of_read_bytes, codigo, endereco);
        }

        codigo++;
        endereco++;
    }
    close(fArt);
    close(fStr);
}
