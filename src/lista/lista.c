/*!
 * @header      lista.c
 * @abstract    Simula "ls"
 * @discussion  Pode ser chamado passando 1 argumento
 *              argumento 1: directorio
 *              caso não seja passado o argumento o directorio é o .
 * 
 * @author      Daniel Torres <a17442|at|alunos.ipca.pt>
 * @link        Daniel Torres GIT <https://github.com/nargotik>
 * @version     1.0 02 de Fevereiro de 2019
 * 
 * @ref         https://linux.die.net/man/2/stat
 *              http://man7.org/linux/man-pages/man3/readdir.3.html
 *              https://www.gnu.org/software/libc/manual/html_node/Testing-File-Type.html
 */

#include <string.h> // strcpy()
#include <stdlib.h> // EXIT_FAILURE, EXIT_SUCCESS
#include <dirent.h>
#include <sys/stat.h> // para o stat() // S_ISDIR Macro
#include <sys/types.h>
#include <stdio.h> // puts() printf
#include <limits.h> // PATH_MAX

int directory_exists(const char *path);

/*
 * Main function
 */
int main(int argc, char** argv) {
    
    char directorio[PATH_MAX];
    
    if (argc == 1) {
        // Não foi passado segundo argumento assume ./
        strcpy(directorio,".");
    } else if (argc == 2) {
        // 
        strcpy(directorio,argv[1]);
    } else {
        puts("Parametros inválidos!");
        puts("Utilizar: lista [DIR]\n");
        return(EXIT_FAILURE);
    }

    if (!directory_exists(directorio)) {
        // O directorio nao existe não existe / ou é um ficheiro
        puts("Directorio inválido!\n");
        return(EXIT_FAILURE);
    } 
    
    // Armazena o tipo de ficheiro
    char tipo[1];
    
    // Dir handler
    DIR *dir;
    
    // Armazena cada entrada do directorio
    struct dirent *entrada;

    dir = opendir(directorio);
      
    while ((entrada = readdir(dir))) {
        switch (entrada->d_type) {
              case DT_REG: // DT_REG - A regular file.
                  tipo[0] = 'F';
                  break;
              case DT_DIR: // DT_DIR - A directory.
                  tipo[0] = 'D';
                  break;
              default:
                  tipo[0] = 'U';
        }
        printf("[%c]\t %s \t\n",tipo[0], entrada->d_name);
    }

    closedir(dir);

    return EXIT_SUCCESS;
}


/**
 * Verifica se o directorio existe e se é um directorio
 * @param path
 * @return 1 se for directorio e existir
 */
int directory_exists(const char *path) {
    struct stat filestats;
    stat(path, &filestats);
    return S_ISDIR(filestats.st_mode);
}