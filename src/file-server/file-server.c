/*!
 * @header      file-server.c
 * @abstract    Exemplo Servidor de ficheiros TCP/IP
 * @discussion  
 *              Servidor TCP/IP para partilha de ficheiros
 *              Pode ser testado com o comando nc
 *              bash$ echo GET /etc/hosts | nc 127.0.0.1 8080
 * 
 * @author      Daniel Torres <a17442|at|alunos.ipca.pt>
 * @link        Daniel Torres GIT <https://github.com/nargotik>
 * @version     1.0 02 de Fevereiro de 2019
 * 
 * @ref         https://www.geeksforgeeks.org/socket-programming-cc/
 *              
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
#include <sys/wait.h> 

#define MAX_CMD_LEN 128
#define FILE_BUFFER 128
#define NEW_LINE '\n'
#define KEY_SPACE ' '


char ** separa(char* cmd);
int file_exists(char *filename);

/*
 * Main function
 */
int main(int argc, char** argv) {
    if (argc != 2) {
        printf("Parametros inválidos!\n"
                "%s [PORT]\n"
                "PORT: number between 1024-49151\n"
                "",argv[0]);
        return(EXIT_FAILURE);
    }
    // Numero da porta
    int port = atoi(argv[1]);
    if ( !(port>=1024 && port<=49151) ) {
        printf("Porta Inválida!\n"
                "%s [PORT]\n"
                "PORT: number between 1024-49151\n"
                "",argv[0]);
        return(EXIT_FAILURE);
    }
    
    // Fim da validação dos argumentos ....
    
    // File Descriptor do cliente e do servidor
    int servidor_fd, cliente_fd;
    
    // Struct do servidor e do cliente 
    struct sockaddr_in server_address, client_address; 
    
    int opt = 1; 
    
    if ((servidor_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) { 
        puts("socket() Falhou"); 
        return(EXIT_FAILURE); 
    } 
    // Forcefully attaching socket to the port  
    if (setsockopt(servidor_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) { 
        puts("setsockopt() Falhou"); 
        return(EXIT_FAILURE); 
    } 
    
    server_address.sin_family      = AF_INET; 
    server_address.sin_addr.s_addr = INADDR_ANY; 
    server_address.sin_port        = htons(port); 
       
    // Forcefully attaching socket to the port 8080 
    if (bind(servidor_fd, (struct sockaddr *)&server_address,sizeof(server_address))<0) { 
        puts("bind Falhou"); 
        return(EXIT_FAILURE); 
    } 
    
    if (listen(servidor_fd, 3) < 0) { 
        puts("listen() Falhou"); 
        return(EXIT_FAILURE); 
    } 
    printf("Servidor à escuta na porta %d\n", port);
    
    int numero_cliente = 0;
    
    while(1) {
        numero_cliente++;
        
        // While de espera de clientes
        socklen_t client_len = sizeof(client_address);
        cliente_fd = accept(servidor_fd, (struct sockaddr *)&client_address, &client_len);

        pid_t child;
        
        // Cria um subprocesso para o novo cliente tcp
        child = fork();
        if(child >= 0) {
            // Fork was successful
            if(child == 0) // Child process
            {
                if (cliente_fd < 0) {
                    puts("Client ?: Não foi possivel estabelecer ligação\n");
                    return EXIT_FAILURE;
                }
                    
                printf("\nCliente %d: conectado sending welcome...\n", numero_cliente);
                //send(cliente_fd, "WELCOME\n", 8, 0);
                
                char * cmd = malloc(MAX_CMD_LEN);
                             
                while (1) {
                    
                    // While de espera de informação do cliente
                    int readed_bytes = recv(cliente_fd, cmd, MAX_CMD_LEN, MSG_NOSIGNAL);

                    if (readed_bytes==0) 
                        break; // Nada para ler mais 

                    if (readed_bytes < 0) 
                        printf("Client %d: Erro ao ler dados do cliente\n", numero_cliente);
                    
                    cmd[ strlen(cmd)-1 ] = '\0';
                    
                    printf("Client %d: Comando recebido (%d): %s \n", numero_cliente, readed_bytes,cmd);
                    
                    // parse do comando
                    char ** args = separa(cmd);
                  
                    if (strcmp(args[0],"GET") == 0) {
                        char *filename = args[1];
                        
                        printf("Client %d: GET: \"%s\"\n", numero_cliente,filename);
                        
                        if (file_exists(filename)) {
                            // Envia o ficheiro
                            int fd;
                            fd = open(filename, O_RDONLY);
                            if (fd < 0) {
                                printf("Client %d: Impossivel abrir o ficheiro para leitura!\n",numero_cliente);
                                return(EXIT_FAILURE);
                            }
                            // Cria um buffer de READ_BUFFER_SIZE byte
                            char * read_buff = malloc(FILE_BUFFER);
                            // Bytes lidos
                            ssize_t fd_readed_bytes = 0;

                            // Enquanto o numero de bytes lidos não fôr zero ...
                            while ((fd_readed_bytes = read(fd,&read_buff,sizeof(read_buff)))) {
                                int sent_bytes = send(cliente_fd, &read_buff, fd_readed_bytes, 0);
                                if (sent_bytes < 0) 
                                    printf("Client %d: Erro ao enviar dados ao cliente\n", numero_cliente);
                            }
                            // Fecha o ficheiro
                            close(fd);

                        } else {
                            // Nada
                            printf("Client %d: filename not exists %s\n", numero_cliente, filename);
                        }
                        // Comando GET
                        
                        
                    }
                    exit(EXIT_SUCCESS);
                } 
                // Apenas aceita um comando ...
                exit(EXIT_SUCCESS);

            } else {
                // Com o wait apenas trata de um cliente de cada vez
                //int status;
                //int w = waitpid(child,&status,WCONTINUED);
                
                close(cliente_fd);
                //exit(1);
            }
        }
    }
    return EXIT_SUCCESS;
}

/**
 * Função que separa ums string em um array de strings pelo delimitador " "
 * 
 * @param texto a separar
 * @return array de strings 
 */
char ** separa(char* string) {
        
    char* aux_string;
    
    char ** ret = malloc(sizeof (char *) * 1);
    
    int argumentos = 0;
    aux_string = strtok(string, " ");
    while (aux_string != NULL) {
        ret[argumentos] = aux_string;
        
        aux_string = strtok(NULL, " ");
        argumentos++;
        // Aumenta o tamanho do array
        ret = realloc(ret,sizeof (char *) * argumentos + 1);
        // Coloca o proximo como NULL
        ret[argumentos] = NULL;
    }
    
    return ret;
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


