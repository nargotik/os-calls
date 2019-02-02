/*!
 * @header      mostra.c
 * @abstract    Simula "cat ficheiro"
 * @discussion  Deve ser chamado passando 1 argumento
 *              argumento 1: ficheiro
 * 
 * @author      Daniel Torres <a17442|at|alunos.ipca.pt>
 * @link        Daniel Torres GIT <https://github.com/nargotik>
 * @version     1.0 02 de Fevereiro de 2019
 * 
 */

#include <string.h> // strlen()
#include <stdlib.h> // EXIT_FAILURE, EXIT_SUCCESS
#include <unistd.h> // read() write() open() ...

#include <fcntl.h> // O_RDONLY || O_RDWR || O_WRONLY

#include <sys/stat.h> // para o stat()
#include <sys/types.h>
#include <stdio.h> // puts()

// Definir em bytes o tamanho máximo a ser usado no buffer de leitura
#define READ_BUFFER_SIZE 128

int file_exists(char *filename);

/*
 * 
 */
int main(int argc, char** argv) {
    
    if (argc != 2) {
        puts("Parametros inválidos!\n");
        return(EXIT_FAILURE);
    }
    
    if (!file_exists(argv[1])) {
        // O ficheiro não existe
        puts("Ficheiro Inexistente!\n");
        return(EXIT_FAILURE);
    } 
    
    int fd;
    fd = open(argv[1], O_RDONLY);
    
    if (fd < 0) {
        puts("Impossivel abrir o ficheiro para leitura!\n");
        return(EXIT_FAILURE);
    }
    
    // Cria um buffer de READ_BUFFER_SIZE byte
    char buff[READ_BUFFER_SIZE];
    
    // Bytes lidos
    ssize_t readed_bytes = 0;
    
    // Enquanto o numero de bytes lidos não fôr zero ...
    while ((readed_bytes = read(fd,&buff,sizeof(buff)))) {
        write(STDOUT_FILENO,&buff, readed_bytes);
    }
    
    // Fecha o ficheiro
    close(fd);

    return EXIT_SUCCESS;
}


/**
 * Verifica se um ficheiro existe
 * 
 * On success, zero is returned. 
 * On error, -1 is returned
 * 
 * @param filename
 * @return o valor boleano da funcao stat(...)==0 que indica que existe
 */
int file_exists(char *filename) {
    // Struct stat contem dados relativos a um ficheiro
    // Esta estrutura é necessário para usar o comando stat
    // http://man7.org/linux/man-pages/man2/fstat.2.html
    struct stat filestats;   
    return ( stat(filename, &filestats) == 0 );
}


