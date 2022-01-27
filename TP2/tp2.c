#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <math.h>
#include "heap.h"
#include "hash.h"
#include "abb.h"
#include "tipos_datos.h"
#include "txt.h"



/* *****************************************************************
 *                CODIGO DEL PROGRAMA 
 * *****************************************************************/

void imprimir_error(size_t codigo_error){
    if(codigo_error == 0){
        fprintf(stdout,"Error: Ya habia un usuario loggeado\n");
        return;
    }
    if(codigo_error == 1){
        fprintf(stdout,"Error: usuario no existente\n");
        return;
    }
    if(codigo_error == 2){
        fprintf(stdout,"Error: no habia usuario loggeado\n");
        return;
    }
    if(codigo_error == 3){
        fprintf(stdout, "Usuario no loggeado o no hay mas posts para ver\n");
        return;
    }
    if(codigo_error == 4){
        fprintf(stdout, "%s\n", "Error: Usuario no loggeado o Post inexistente");
        return;
    }
    if(codigo_error == 5){
        fprintf(stdout, "%s\n", "Error: Post inexistente o sin likes");
        return;
    }
}


void imprimir_mensaje(size_t codigo_mensaje, char* contenido){
    if(codigo_mensaje == 0){
        fprintf(stdout,"Hola %s", contenido);
        return;
    }
    if(codigo_mensaje == 1){
        fprintf(stdout,"Adios\n");
        return;
    }
    if(codigo_mensaje == 2){
        fprintf(stdout, "%s",contenido);
        return;
    }
    if(codigo_mensaje == 3){
        fprintf(stdout, "Post publicado\n");
        return;
    }
    if(codigo_mensaje == 4){
        fprintf(stdout, "Post ID %s\n", contenido);
        return;
    }
    if(codigo_mensaje == 5){
        fprintf(stdout, "%s dijo: ", contenido);
        return;
    }
    if(codigo_mensaje == 6){
        fprintf(stdout, "Likes: %s\n", contenido);
        return;
    }
}


char* login(hash_t* hash_usuarios, char* usuario){
    size_t error;
    if(strcmp(usuario, "") != 0){
        imprimir_error(0);
        return usuario;
    }
    size_t tam = 0;
    char* usuario_getline = "";

    error = getline(&usuario_getline, &tam, stdin);

    if(error == -1){                           //El corrector por mail nos pide no ignorar la salida de 
        free(usuario_getline);                 //getline.
        return NULL;
    }

    if(!hash_pertenece(hash_usuarios, usuario_getline)){
        imprimir_error(1);
    }
    else{
        usuario = strdup(usuario_getline);
        imprimir_mensaje(0, usuario);
    }
    free(usuario_getline);
    return usuario;
}

char* logout(char* usuario){
    if(strcmp(usuario, "") == 0){
        imprimir_error(2);
    }else{
        imprimir_mensaje(1, usuario);
        free(usuario);
        usuario = "";
    }
    return usuario;
}

void destruir_publi(publi_t* publicacion){
    abb_destruir(publicacion->likeadores);
    free(publicacion->descripcion);
    free(publicacion->publicador);
    free(publicacion);
}


void _publi_destruir(void* publicacion){
    destruir_publi((publi_t*)publicacion);
}


void destruir_idenificador(identificador_id_t* identificador_id){
    free(identificador_id);
}


void _identificador_destruir(void* identificador_id){
    destruir_idenificador((identificador_id_t*)identificador_id);
}


void destruir_usuario(usuarios_t* usuario){
    heap_destruir(usuario->feed,_identificador_destruir);
    free(usuario);
}


void _usuario_destruir(void* usuario){
    destruir_usuario((usuarios_t*)usuario);
}

publi_t *crear_publi(char* descripcion, size_t id, char* publicador){
    publi_t *publi = malloc(sizeof(publi_t));
    if(!publi)return NULL;

    publi->descripcion = descripcion;
    publi->id = id;
    publi->publicador = strdup(publicador);
    publi->publicador[strlen(publi->publicador) - 1] = '\0'; //Borrando el /n del final
    publi->likeadores = abb_crear(strcmp, free);

    return publi;
}

size_t calcular_prioridad(size_t indice_actual, size_t indice_us_2){
    int prioridad_post = (int) (indice_us_2 - indice_actual);
    if(prioridad_post < 0) prioridad_post = prioridad_post * (-1);
    return (size_t) prioridad_post;
}

identificador_id_t *crear_identificador(size_t prioridad, size_t id_post){
    identificador_id_t *identificador = malloc(sizeof(identificador_id_t));
    if(!identificador)return NULL;

    identificador->id_post = id_post;
    identificador->prioridad = prioridad;

    return identificador;
}

void publicar_post(hash_t*hash_usuarios, hash_t *publicaciones, char* n_us_actual){
    char* vacio = "";
    if( n_us_actual == vacio){
        imprimir_error(2);
        return;
    }
    char* contenido = "";
    size_t tam2 = 0;

    size_t error;

    error = getline(&contenido, &tam2, stdin);

    if(error == -1){                           //El corrector por mail nos pide no ignorar la salida de 
        free(contenido);                       //getline.
        return;
    }

    size_t id_post = hash_cantidad(publicaciones);
    publi_t *publi = crear_publi(contenido, id_post, n_us_actual);

    char *id_str = malloc(sizeof(char) * 10);
    sprintf(id_str, "%d", (int)id_post);
    hash_guardar(publicaciones, id_str, publi);
    free(id_str);

    hash_iter_t *iter = hash_iter_crear(hash_usuarios);

    while(!hash_iter_al_final(iter)){
        const char *n_usuario = hash_iter_ver_actual(iter);

        usuarios_t *datos_usuario = hash_obtener(hash_usuarios,n_us_actual);
        size_t indice_us_actual = datos_usuario->id_usuario;

        if(strcmp(n_usuario, n_us_actual) != 0){
            usuarios_t *usuario = (usuarios_t*)hash_obtener(hash_usuarios, n_usuario);
            size_t indice_us_2 = usuario->id_usuario;
            size_t prioridad = calcular_prioridad(indice_us_actual, indice_us_2);
            identificador_id_t *identificador = crear_identificador(prioridad, id_post);
            heap_encolar(usuario->feed,identificador);
        }
        hash_iter_avanzar(iter);
    }
    hash_iter_destruir(iter);
    imprimir_mensaje(3, NULL);

}

void ver_siguiente_feed( char *nombre_usuario, hash_t *publicaciones, hash_t *hash_usuarios){
    char* vacio = "";
    if( nombre_usuario == vacio){
        imprimir_error(3);
        return;
    }
    
    usuarios_t *usuario = (usuarios_t*)hash_obtener(hash_usuarios, nombre_usuario);
    heap_t *feed = usuario->feed;

    if( heap_cantidad(feed)==0){
        imprimir_error(3);
        return;
    }


    identificador_id_t *identificador = (identificador_id_t*)heap_desencolar(feed);
    int post_id = (int)identificador->id_post;
    char *id_str = malloc(sizeof(char) * 10);
    sprintf(id_str, "%d", (int)post_id);

    publi_t *post = (publi_t*)hash_obtener(publicaciones, id_str);
    char * descripcion = post->descripcion;
    char *n_likes = malloc(sizeof(char) * 10);
    sprintf(n_likes, "%zu", abb_cantidad(post->likeadores));

    imprimir_mensaje(4, id_str);
    imprimir_mensaje(5, post->publicador);
    imprimir_mensaje(2, descripcion);
    imprimir_mensaje(6, n_likes);
    free(id_str);
    free(n_likes);
    destruir_idenificador(identificador);
}


void likear_post(hash_t *hash_publicaciones, char *usuario){
    char* id_del_post = "";
    size_t tam = 0;

    size_t error = getline(&id_del_post, &tam, stdin);

    if(error == -1){                           //El corrector por mail nos pide no ignorar la salida de 
        free(id_del_post);                     //getline.
        return;
    }

    char* auxiliar = strdup(id_del_post);
    auxiliar[strlen(auxiliar) - 1] = '\0';

    if(strcmp(usuario, "") == 0){
        imprimir_error(4);
        free(id_del_post);
        free(auxiliar);
        return;
    }
    
    if(!hash_pertenece(hash_publicaciones, auxiliar)){
        imprimir_error(4);
        free(id_del_post);
        free(auxiliar);
        return;
    }

    publi_t *post = (publi_t*)hash_obtener(hash_publicaciones, auxiliar);

    if(!abb_pertenece(post->likeadores,usuario)){
        abb_guardar(post->likeadores,usuario, NULL);
    }
    imprimir_mensaje(2,"Post likeado\n");
    free(id_del_post);
    free(auxiliar);
}


bool printear_likeadores(const char* clave, void *valor, void *extra){
    fprintf(stdout,"\t%s", clave);
    return true;
}


bool printear_likeadores_wrapper(const char *clave, void *valor, void *extra){
    return printear_likeadores(clave, valor, extra);
}


void mostrar_likes(hash_t *hash_publicaciones){
    char* id_post_mostrar_likes = "";
    size_t tam3 = 0;

    size_t error = getline(&id_post_mostrar_likes, &tam3, stdin);
    
    if(error == -1){                           //El corrector por mail nos pide no ignorar la salida de 
        free(id_post_mostrar_likes);           //getline.
        return;
    }

    char* auxiliar = strdup(id_post_mostrar_likes);
    auxiliar[strlen(auxiliar) - 1] = '\0';
    
    if(!hash_pertenece(hash_publicaciones, auxiliar)){
        imprimir_error(5);
        free(id_post_mostrar_likes);
        free(auxiliar);
        return;
    }

    publi_t *post = (publi_t*)hash_obtener(hash_publicaciones, auxiliar);

    if(abb_cantidad(post->likeadores) == 0){
        imprimir_error(5);
        free(id_post_mostrar_likes);
        free(auxiliar);
        return;
    } 
    
    fprintf(stdout,"El post tiene %ld likes:\n", abb_cantidad(post->likeadores));
    abb_in_order(post->likeadores, printear_likeadores_wrapper, NULL);
    
    free(auxiliar);
    free(id_post_mostrar_likes);
}



int main(int argc, char* argv[]){
    if(argc < 2){
        return -1;
    }
    char *nombre_archivo = argv[1];
    hash_t *hash_publicaciones = hash_crear(_publi_destruir);
    hash_t *hash_usuarios = hash_crear(_usuario_destruir);
    hash_usuarios = cargar_usuarios(hash_usuarios, nombre_archivo);

    if(!hash_usuarios){
        return -1;
    }

    size_t tam = 0;
    char* opc = "";
    char* usuario = "";

    while ((getline(&opc, &tam, stdin)) != EOF){
        opc[strlen(opc) - 1] = '\0';
        if(strcmp(opc, "login") == 0){
            usuario = login(hash_usuarios, usuario);
        }
        if(strcmp(opc, "logout") == 0){
            usuario = logout(usuario);
        }

        if(strcmp(opc, "publicar") == 0){
            publicar_post(hash_usuarios, hash_publicaciones,usuario);        
        }
        
        if(strcmp(opc, "ver_siguiente_feed") == 0){
            ver_siguiente_feed(usuario, hash_publicaciones, hash_usuarios);
        }
        
        if(strcmp(opc, "likear_post") == 0){
            likear_post(hash_publicaciones, usuario);
        }
    
        if(strcmp(opc, "mostrar_likes") == 0){
            mostrar_likes(hash_publicaciones);
        }  
    }
    free(opc);
    hash_destruir(hash_publicaciones);
    hash_destruir(hash_usuarios);
}