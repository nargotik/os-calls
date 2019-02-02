/*!
 * @header      acrescenta.c
 * @abstract    Simula "cat ficheiro >> ficheiro2"
 * @discussion  Deve ser chamado passando 2 argumentos
 *              argumento 1: ficheiro
 *              argumento 2: ficheiro2
 * 
 * @author      Daniel Torres <a17442|at|alunos.ipca.pt>
 * @link        Daniel Torres GIT <https://github.com/nargotik>
 * @version     1.0 02 de Fevereiro de 2019
 * 
 * @ref         https://linux.die.net/man/2/lseek
 *              http://man7.org/linux/man-pages/man2/fstat.2.html
 */

#include <string.h> // strlen()
#include <stdlib.h> // EXIT_FAILURE, EXIT_SUCCESS
#include <unistd.h> // read() write() open() ...

#include <fcntl.h> // O_RDONLY || O_RDWR || O_WRONLY

#include <sys/stat.h> // para o stat()
#include <sys/types.h>
#include <stdio.h> // puts() printf


// Definir em bytes o tamanho maximo a ser usado no buffer de leitura
#define READ_BUFFER_SIZE 12

int file_exists(char *filename);

/*
 * Main function
 */
int main(int argc, char** argv) {
    
    if (argc != 3) {
        puts("Parametros inválidos!\n");
        return(EXIT_FAILURE);
    }
    
    if (!file_exists(argv[1])) {
        // O ficheiro não existe
        puts("Ficheiro1 Inexistente!\n");
        return(EXIT_FAILURE);
    } 
    
    if (!file_exists(argv[2])) {
        // O ficheiro não existe
        puts("Ficheiro2 Inexistente!\n");
        return(EXIT_FAILURE);
    } 
    
    // Fim da validação dos argumentos ....
    
    int fd1, fd2;
    
    // Abre o ficheiro para escrita
    fd1 = open(argv[1], O_WRONLY);
    // Coloca o posicionador no final do ficheiro e verifica o tamanho
    size_t tamanhof1 = (size_t)lseek(fd1,0,SEEK_END);
    
    // Abre o ficheiro 2 para leitura
    fd2 = open(argv[2], O_RDONLY);
    
    // Coloca o apontador no fim do ficheiro e vê o tamanho
    size_t tamanhof2 = (size_t)lseek(fd2,0,SEEK_END); // converte off_t para size_t
    // Recoloca o apontador no inicio
    lseek(fd2,0,SEEK_SET);
    
    if (fd1 <= 0 || fd2 <= 0) {
        puts("Impossivel abrir os ficheiros selecionados!\n");
        return(EXIT_FAILURE);
    }
    
    // Cria um buffer
    char buff[READ_BUFFER_SIZE];
    
    // Bytes lidos
    ssize_t readed_bytes = 0;
    
    size_t writed = 0;

    // Enquanto o numero de bytes lidos não fôr zero ...  
    while ((readed_bytes = read(fd2, &buff, sizeof(buff)))) {
         // Escreve no ficheiro fd1 o buffer de readed_bytes
         writed += write(fd1, &buff, readed_bytes); 
    }
    
    printf("Foram escritos %lu Bytes no ficheiro %s(%lu bytes) do ficheiro %s(%lu bytes) (buffer %lu Bytes)\n",
            (unsigned long)writed, 
            argv[1], 
            (unsigned long)tamanhof1,
            argv[2], 
            (unsigned long)tamanhof2,
            (unsigned long)sizeof(buff)
          );

    // Fecha o ficheiro de escrita
    close(fd1);
    
    // Fecha o ficheiro de leitura
    close(fd2);

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
    // Esta estrutura é necessaria para usar o comando stat
    // http://man7.org/linux/man-pages/man2/fstat.2.html
    struct stat filestats;   
    return ( stat(filename, &filestats) == 0 );
}


