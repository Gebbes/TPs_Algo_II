#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>


void embutidora(int buffer, FILE *archivo){
    char* linea = NULL;
    size_t tam;
    int iter = 0;
    int vueltas = 0;
    
    while( getline(&linea, &tam, archivo) != EOF){
        while( linea[iter] != '\n' && linea[iter] != '\0'){
                if((iter - vueltas * buffer) == buffer){
                fprintf(stdout,"\n%c", linea[iter]);
                vueltas += 1;
                }else{
                fprintf(stdout,"%c", linea[iter]);
                }
                iter += 1;
            }
        if( linea[iter] == '\n' && linea[iter] != '\0'){
            fprintf(stdout,"%c",linea[iter]);
        }
        iter = 0;
        vueltas = 0;
    }
    free(linea);
}


int main(int argc, char *argv[]){

    if( !((argc == 2 ) || (argc == 3 )) ||  (!isdigit(*argv[1])) ){
        fprintf(stderr, "%s", "Error: Cantidad erronea de parametros");
        return 1;
    }

    int n_chars = atoi(argv[1]);

    if(n_chars == 0){
        fprintf(stderr,"%s","Error: 0 no es una cantidad valida");
        return 1;
    }

    if(argc == 3){
        FILE *archivo = fopen(argv[2], "r");
        if(!archivo){
            fprintf(stderr, "%s", "Error: archivo fuente inaccesible");
        return 1;
        }
        embutidora(n_chars , archivo);
        fclose(archivo);
        return 0;
        
    }

    embutidora(n_chars,stdin);
}
