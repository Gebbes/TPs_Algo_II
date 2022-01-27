#include <stdlib.h>
#include "heap.h"
#include "abb.h"

/* *****************************************************************
 *                DEFINICION DE LOS TIPOS DE DATOS
 * *****************************************************************/

typedef struct identificador{
    size_t id_post;                       // BORRE LOS CONST
    size_t prioridad;
}identificador_id_t;

typedef struct usuarios{
    heap_t* feed;
    size_t id_usuario;
}usuarios_t;


typedef struct publicacion{
    size_t id;
    char* publicador;
    char* descripcion;
    abb_t* likeadores;
}publi_t;

/*
hash_usuario, las claves son los nombres de los usuarios y el dato el struct usuarios_t
hash_publi, las claves son 
*/