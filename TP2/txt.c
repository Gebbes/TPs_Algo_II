#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include "heap.h"
#include "hash.h"
#include "tipos_datos.h"
#include "txt.h"

#define K_LECTURA 80

int comparar_identificadores(identificador_id_t* iden_1, identificador_id_t* iden_2){
    if(iden_1->prioridad < iden_2->prioridad) return 1; // Numeros menores de valor son mas alta prioridad
    if(iden_1->prioridad > iden_2->prioridad) return -1;

    if(iden_1->id_post < iden_2->id_post)return 1; // Numeros menores de valor son mas alta prioridad
    if(iden_1->id_post > iden_2->id_post)return -1;

    return 0;
}


int identificador_cmp(const void* iden_1, const void* iden_2){
    return comparar_identificadores((identificador_id_t*)iden_1, (identificador_id_t*)iden_2);
}


usuarios_t *crear_usuario(size_t id){
    usuarios_t* usuario = malloc(sizeof(usuarios_t));
    usuario->id_usuario = id;
    usuario->feed = heap_crear(identificador_cmp);
    return usuario;
}


hash_t* cargar_usuarios(hash_t* hash_usuarios, char *nombre_archivo){
    FILE *mi_archivo = fopen(nombre_archivo, "r");

    if(mi_archivo == NULL){
        fprintf(stderr, "Error: archivo fuente inaccesible");
        return NULL;
    } 
    
    char* linea = "";
    size_t tam = 0;
    size_t cont = 0;

    while((getline(&linea, &tam, mi_archivo)) != EOF){  
        usuarios_t *us = crear_usuario(cont); 
        hash_guardar(hash_usuarios, linea, us);
        
        cont ++;
    }
    free(linea);
    fclose(mi_archivo);

    return hash_usuarios;
}