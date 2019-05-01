#include "ma.h"

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include "ma.h"

void insereArtigos(int fd,int argcMA, char* argvMA[], size_t number_of_read_bytes){
    write(fd, argvMA[1], number_of_read_bytes);
}
void alteraArtigo(int fd, int argcMA, char* argvMA[]){
    printf("n\n");
}
void alteraPreco(int fd, int argc, char* argvMA[]){
    printf("p\n");
}



void manutencao_artigos(){
    size_t number_of_read_bytes;
    int bytes_to_read = 64;
    char *buffer = (char*)malloc(bytes_to_read * sizeof(char));
    int argcMA;
    char **argvMA = (char**)malloc((bytes_to_read/2) * sizeof(char*));
    int fd = open("ARTIGOS.txt", O_CREAT | O_RDWR, 0666);
    int fd2 = open("STRINGS.txt", O_CREAT | O_RDWR, 0666);
    int i;
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
            insereArtigos(fd, argcMA,argvMA, number_of_read_bytes);
        } else if(strcmp (argvMA[0],"n") == 0){
            alteraArtigo(fd,argcMA, argvMA);
        } else if (strcmp (argvMA[0],"p") == 0){
            alteraPreco(fd,argcMA, argvMA);
        }
    }
}

int main(int argc, char* argv[]){
    manutencao_artigos();
    return 0;
}

