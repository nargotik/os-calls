/*!
 * @header      interpretador.c
 * @abstract    Consola Básica de Comandos
 * @discussion  
 *              Consola básica pedido no trabalho prático de 
 *              Sistemas Operativos e Sistemas Distribuidos
 * 
 *              A consola não aceita multiplos comandos numa só linha
 *              o mesmo necessitaria da divisão dos comandos pelo operador |
 *              e execução dos mesmos.
 *              Como não foi pedido no enunciado e para não tornar o codigo 
 *              complexo nesta versao foi optado por não dar essa funcionalidade
 * 
 *              Para sair da consola deve-se colocar o comando:
 *              % termina
 * 
 *              Atencão! 
 *              Tal como no sistema operativo linux todos os comandos 
 *              são case-sensitive
 * @author      Daniel Torres <a17442|at|alunos.ipca.pt>
 * @link        Daniel Torres GIT <https://github.com/nargotik>
 * @version     1.0 02 de Fevereiro de 2019
 * 
 * @ref         https://www.gnu.org/software/libc/manual/html_node/Testing-File-Type.html
 *              http://man7.org/linux/man-pages/man2/wait.2.html
 *              https://www.geeksforgeeks.org/wait-system-call-c/
 */

#include <string.h> // strcpy()
#include <stdlib.h> // EXIT_FAILURE, EXIT_SUCCESS
#include <dirent.h>
#include <sys/stat.h> // para o stat() // S_ISDIR Macro
#include <sys/types.h>
#include <stdio.h> // puts() printf
#include <limits.h> // PATH_MAX
#include <unistd.h> // STDIN_FILENO
#include <sys/wait.h> 

#define MAX_CMD_LEN 128
#define NEW_LINE '\n'
#define KEY_SPACE ' '


char ** separa(char* cmd);

/*
 * Main function
 */
int main() {
    size_t readed = 0;
    
    puts("Interpretador Start");
    while (1) {
        // Coloca o % escrever no STDOUT_FILENO
        write(STDOUT_FILENO, "\033[0;31m",sizeof("\033[0;31m")); // cor vermelha
        write(STDOUT_FILENO, "% ",sizeof("% "));
        write(STDOUT_FILENO, "\033[0m",sizeof("\033[0m")); // reser à cor
        
        char buffer[1];
        int i = 0;
        char* command = (char *)malloc(MAX_CMD_LEN);
       
        // Leitura dos comandos através do read()
        while((readed = read(STDIN_FILENO, &buffer, 1)) > 0) {
            // @todo verificar se o i corresponde à ultima posição disponivel da string command
            
            if (buffer[0] == NEW_LINE) {
                // Se for uma newline sai fora do while read
                command[i] = '\0';
                break;
            } else {
                command[i] = buffer[0];
                i++;
            }
            //@debug printf("%c",buffer[0]);
        }
        
        // @debug printf("%s",command);
        // Separa o comando para um array de argumentos o args[0] 'e o comando
        char ** args = separa(command);
        
        // Comando de terminar
        // o break; sai do while(1)...
        if (strcmp(args[0],"termina") == 0) 
            break;
           
        // String do comando recebida e separada
        // Parte do codigo de execução do comando
        pid_t   pid;
        int status;
        int result;
        
        if ((pid = fork()) < 0) {
            /* fork a child process */
            puts("Erro ao fazer fork");
            exit(1);
        } else if (pid == 0) {
            // Executa o processo e termina com o result
            result = execvp(args[0], args);
            
            // Se chegar a este ponto algo falhou...
            puts("Erro ao executar a aplicação");
            
        } else {
            // Processo pai
            do {
                // Ciclo de espera pelo processo filho ...
                int w = wait(&status);
                if (w == -1) {
                    printf("Erro na espera do fim do processo %s",args[0]);
                    exit(EXIT_FAILURE);
                }
                
                if (WIFEXITED(status)) {
                    printf("Terminou o comando %s com código %d\n",args[0], WEXITSTATUS(status));
                } else if (WIFSIGNALED(status)) {
                    printf("killed by signal %d\n", WTERMSIG(status));
                } else if (WIFSTOPPED(status)) {
                    printf("stopped by signal %d\n", WSTOPSIG(status));
                } else if (WIFCONTINUED(status)) {
                    printf("continued\n");
                }
                
            } while (!WIFEXITED(status) && !WIFSIGNALED(status));
            // Espera pelo terminos do processo
        }
        //@debug printf("\nCommand: %s\n",args[2]);
        free(command);
        free(args);

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