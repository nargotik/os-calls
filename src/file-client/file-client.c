/*!
 * @header      file-client.c
 * @abstract    Exemplo Cliente de ficheiros TCP/IP
 * @discussion  
 *              Cliente TCP/IP para partilha de ficheiros
 *              Pode ser testado com o comando nc
 *              bash$ nc -l -p 8000
 * 
 * @author      Daniel Torres <a17442|at|alunos.ipca.pt>
 * @link        Daniel Torres GIT <https://github.com/nargotik>
 * @version     1.0 02 de Fevereiro de 2019
 * @ref         
 *              https://www.geeksforgeeks.org/socket-programming-cc/
 */

#include <string.h> // strcpy()
#include <stdlib.h> // EXIT_FAILURE, EXIT_SUCCESS, atoi
#include <dirent.h>
#include <sys/stat.h> // para o stat() // S_ISDIR Macro
#include <sys/types.h>
#include <stdio.h> // puts() printf
#include <limits.h> // PATH_MAX
#include <unistd.h> // STDIN_FILENO
#include <fcntl.h>

#include <sys/socket.h> 
#include <netinet/in.h>
#include <netdb.h>  // hostent struct

#define MAX_CMD_LEN 128
#define FILE_BUFFER 128
#define NEW_LINE '\n'
#define KEY_SPACE ' '
#define GET_CMD "GET "



int file_exists(char *filename);

/*
 * Main function
 */
int main(int argc, char** argv) {
    int sock_fd;
    int fd1;
    struct hostent *hostnm;
    struct sockaddr_in server_addr;
    
    char cmd[MAX_CMD_LEN];
    
    if (argc < 4) {
        printf("Parametros inválidos!\n"
                "%s [SERVER] [PORT] FILE [1|0(default) - SAVE] [1|0(default)-SHOW]\n"
                "PORT: number between 1024-49151\n"
                "",argv[0]);
        return(EXIT_FAILURE);
    }

    // Parametro de gravar
    int save = (argc > 4 ? atoi(argv[4]) : 0);
    if (!(save>=0 && save <=1)) {
        puts("Parametro de gravar inválido!\n");
        exit(EXIT_FAILURE);
    }
    
    // Parametro de gravar
    int show = (argc > 5 ? atoi(argv[5]) : 0);
    if (!(show>=0 && show <=1)) {
        puts("Parametro de mostrar inválido!\n");
        exit(EXIT_FAILURE);
    }
    
    char *filename = argv[3];
    // Para nao fazer overwrite ao ficheiro 
    if (save == 1 && file_exists(filename)) {
        puts("Ficheiro já existente no cliente!\n");
        exit(EXIT_FAILURE);
    }
    
    // prevenir tamanhos de ficheiros superiores ao tamanho do comando maximo
    if (strlen(filename) >= (MAX_CMD_LEN - (strlen(GET_CMD) + 1))) {
        puts("Nome do ficheiro demasiado longo!\n");
        exit(EXIT_FAILURE);
    }

    // Numero da porta
    int port = atoi(argv[2]);
    if ( !(port>=1024 && port<=49151) ) {
        puts("Porta Inválida!\n");
        exit(EXIT_FAILURE);
    }

    // Fim da validação dos argumentos ....
    
    hostnm = gethostbyname(argv[1]);
    if (hostnm == NULL) {
      puts("Dominio não resolvido ...\n");
      exit(EXIT_FAILURE);
    }
    
    server_addr.sin_family      = AF_INET;
    server_addr.sin_port        = htons(port);
    server_addr.sin_addr.s_addr = *((unsigned long *)hostnm->h_addr);

    
    if ((sock_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        puts("Socket()");
        exit(EXIT_FAILURE);
    }
    
    if (connect(sock_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0){
        puts("Falhou a conectar");
        exit(EXIT_FAILURE);
    }
   
    // Forma o comando
    sprintf(cmd,"%s%s\n",GET_CMD,filename);

   
    
    if (send(sock_fd, cmd, sizeof(cmd), 0) < 0) {
        puts("Falha ao pedir o ficheiro");
        exit(EXIT_FAILURE);
    }

    
    size_t writed = 0;
    size_t received = 0;
    size_t readed_bytes = 0;
    
    char recv_buffer[1];
    while ( (readed_bytes = recv(sock_fd, recv_buffer, sizeof(recv_buffer), 0)) ) {
        if (save == 1) {
            // Save procedure
            if (writed == 0 && readed_bytes==1) {
                // First Byte Create file
                fd1 = open(filename, O_WRONLY | O_CREAT | O_EXCL, 0644);
                if (fd1 <= 0) {
                    printf("%s",filename);
                    puts("Impossivel abrir o ficheiro para escrita!\n");
                    exit(EXIT_FAILURE);
                }
            }
            
            writed += write(fd1, &recv_buffer, readed_bytes); 
        }
        
        if (show == 1) {
            printf("%c",recv_buffer[0]);
        }
        received += readed_bytes;
        
    }
    if (writed > 0) {
        close(fd1);
        printf("Saved %d bytes for file %s\n", (int)writed, filename);
    }
    if (received>0) {
        printf("Received %d bytes from \"%s\" file \"%s\"\n", (int)received, argv[1], filename);
    } else {
        puts("Failed to receive file (Server didnt respond).");
    }
    
    
    if (writed != received || received == 0) 
    
    close(sock_fd);
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


