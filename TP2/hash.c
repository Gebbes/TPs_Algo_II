#define _POSIX_C_SOURCE 200809L
#include "hash.h"
#include <string.h>
#include <stdlib.h>

#define CAPACIDAD 15
#define FACTOR_DE_CARGA 0.20
#define FACTOR_REDIMENSION 2
#define ENCONTRADO_FALSE -1

typedef enum{VACIO, OCUPADO, BORRADO} estado_t;

typedef struct campo{
    char* clave;
    void* dato;
    estado_t estado;
}campo_t;

struct hash{
    size_t cantidad;
    size_t capacidad;
    hash_destruir_dato_t destruir_dato;
    campo_t* tabla;
};


struct hash_iter{
    const hash_t *hash;
    size_t indice;
};

/* *****************************************************************
 *                    PRIMITIVAS DEL HASH
 * *****************************************************************/

 // Buscamos en google "best hash function c" y fue la primera pagina que aparecio usamos el sdbm porque parece el mas raro entre los 3 http://www.cse.yorku.ca/~oz/hash.html 
static unsigned long hashing(const char* clave){
    unsigned long hash = 0;
	int c;

	while ((c = *clave++)) {
		hash = c + (hash << 6) + (hash << 16) - hash;
	}

	return hash;
}
/*Tambien encontramos estos hash que son mejores pero como dependen si el sistema es de 32 o 64 bits preferimos no usarlos
https://stackoverflow.com/questions/7666509/hash-function-for-string (en la pagina hay una tabla con los tiempos de cada funcion de hashing):

Hash function    | collision rate | how many minutes to finish
==============================================================
MurmurHash3      |           6.?% |                      4m15s
Jenkins One..    |           6.1% |                      6m54s   
Bob, 1st in link |          6.16% |                      5m34s
SuperFastHash    |            10% |                      4m58s
bernstein        |            20% |       14s only finish 1/20
one_at_a_time    |          6.16% |                       7m5s
crc              |          6.16% |                      7m56s

uint32_t inline MurmurOAAT32 ( const char * key)
{
  uint32_t h(3323198485ul);
  for (;*key;++key) {
    h ^= *key;
    h *= 0x5bd1e995;
    h ^= h >> 15;
  }
  return h;
}

uint64_t inline MurmurOAAT64 ( const char * key)
{
  uint64_t h(525201411107845655ull);
  for (;*key;++key) {
    h ^= *key;
    h *= 0x5bd1e9955bd1e995;
    h ^= h >> 47;
  }
  return h;
}
*/

bool hash_redimensionar(hash_t * hash, size_t capacidad){
    campo_t* nueva_tabla = malloc(sizeof(campo_t)* capacidad);

    if(!nueva_tabla){ return false;}

    size_t capacidad_aux = hash->capacidad;
    campo_t* tabla_aux = hash->tabla;

    hash->capacidad = capacidad;
    hash->tabla = nueva_tabla;
    hash->cantidad = 0;

    for (size_t i = 0; i < capacidad; i++){
        nueva_tabla[i].clave = NULL;
        nueva_tabla[i].dato = NULL;
        nueva_tabla[i].estado = VACIO;
    }

    for(size_t i = 0; i < capacidad_aux; i++){
        if(tabla_aux[i].estado == OCUPADO){
            hash_guardar(hash, tabla_aux[i].clave, tabla_aux[i].dato);
            free(tabla_aux[i].clave);
        }
    }
    free(tabla_aux);
    return true;
}

size_t buscar_clave(const hash_t *hash, size_t posicion, const char *clave, bool buscar){

    for (size_t i = 0; i < hash->capacidad; i++){
        posicion =(posicion + i*i) % hash->capacidad;
        if(buscar && (hash->tabla[posicion].estado == OCUPADO) && (strcmp(hash->tabla[posicion].clave, clave) == 0)){
            return posicion;
        }
        if(!buscar && (hash->tabla[posicion].estado != OCUPADO)){
            return posicion;
        }
    }
    return ENCONTRADO_FALSE;
}

/* Crea el hash
 */
hash_t *hash_crear(hash_destruir_dato_t destruir_dato){

    hash_t *hash = malloc(sizeof(hash_t));
    if(!hash){return NULL;}

    hash->tabla = malloc(sizeof(campo_t) * CAPACIDAD);

    if(!hash->tabla){ 
        free(hash);
        return NULL;
    }

    for (size_t i = 0; i < CAPACIDAD; i++){
        hash->tabla[i].clave = NULL;
        hash->tabla[i].dato = NULL;
        hash->tabla[i].estado = VACIO;

    }

    hash->cantidad = 0;
    hash->capacidad = CAPACIDAD;
    hash->destruir_dato = destruir_dato;

    return hash;
}

/* Guarda un elemento en el hash, si la clave ya se encuentra en la
 * estructura, la reemplaza. De no poder guardarlo devuelve false.
 * Pre: La estructura hash fue inicializada
 * Post: Se almacenó el par (clave, dato)
 */
bool hash_guardar(hash_t *hash, const char *clave, void *dato){

    if(((double)hash->cantidad/ (double)hash->capacidad) >= FACTOR_DE_CARGA){

        if(!hash_redimensionar(hash, hash->capacidad * FACTOR_REDIMENSION)){
            return false;
        } 

    }

    size_t indice = hashing(clave);
    size_t posicion = hashing(clave)% hash->capacidad;

    if((hash->tabla[posicion].estado == OCUPADO) && (strcmp(hash->tabla[posicion].clave, clave) == 0)){

        if(hash->destruir_dato){
            hash->destruir_dato(hash->tabla[posicion].dato);
        }

    }else {
        if((hash->tabla[posicion].estado == OCUPADO)){
            posicion = buscar_clave(hash, indice, clave, false);
        }
  
        char* clave_aux = strdup(clave);
        hash->tabla[posicion].clave = clave_aux;
        hash->tabla[posicion].estado = OCUPADO;
        hash->cantidad ++;

    }
    hash->tabla[posicion].dato = dato;

    return true;
}

/* Borra un elemento del hash y devuelve el dato asociado.  Devuelve
 * NULL si el dato no estaba.
 * Pre: La estructura hash fue inicializada
 * Post: El elemento fue borrado de la estructura y se lo devolvió,
 * en el caso de que estuviera guardado.
 */
void *hash_borrar(hash_t *hash, const char *clave){
    size_t indice = hashing(clave);
    size_t posicion = buscar_clave(hash, indice, clave, true);
    
    if(posicion == ENCONTRADO_FALSE){ return NULL;}

    void* dato_aux = hash->tabla[posicion].dato;
    hash->tabla[posicion].dato = NULL;
    hash->tabla[posicion].estado = BORRADO;
    free(hash->tabla[posicion].clave);
    hash->cantidad--;

    return dato_aux;
}

/* Obtiene el valor de un elemento del hash, si la clave no se encuentra
 * devuelve NULL.
 * Pre: La estructura hash fue inicializada
 */
void *hash_obtener(const hash_t *hash, const char *clave){
    size_t indice = hashing(clave);
    size_t posicion = buscar_clave(hash, indice, clave, true);
    
    if(posicion == ENCONTRADO_FALSE){ return NULL;}

    return hash->tabla[posicion].dato;
}

/* Determina si clave pertenece o no al hash.
 * Pre: La estructura hash fue inicializada
 */
bool hash_pertenece(const hash_t *hash, const char *clave){

    size_t indice = hashing(clave);
    size_t posicion = buscar_clave(hash, indice, clave, true);
    
    if(posicion == ENCONTRADO_FALSE){ return false;}

    return true;
}

/* Devuelve la cantidad de elementos del hash.
 * Pre: La estructura hash fue inicializada
 */
size_t hash_cantidad(const hash_t *hash){
    return hash->cantidad;
}

/* Destruye la estructura liberando la memoria pedida y llamando a la función
 * destruir para cada par (clave, dato).
 * Pre: La estructura hash fue inicializada
 * Post: La estructura hash fue destruida
 */
void hash_destruir(hash_t *hash){
    for(size_t i = 0; i < hash->capacidad; i++){
        if(hash->tabla[i].estado == OCUPADO){
            free(hash->tabla[i].clave);
            if(hash->destruir_dato){
                hash->destruir_dato(hash->tabla[i].dato);
            }
        }
    }
    free(hash->tabla);
    free(hash);
}

/* *****************************************************************
 *                    PRIMITIVAS DEL ITERADOR
 * *****************************************************************/

hash_iter_t *hash_iter_crear(const hash_t *hash){
    hash_iter_t *iter = malloc(sizeof(hash_iter_t));
    if(!iter)return NULL;
    iter->hash = hash;
    iter->indice = -1;
    hash_iter_avanzar(iter);
    return iter;
}


bool hash_iter_avanzar(hash_iter_t *iter){
    bool buscando = true;
    while(buscando){
        iter->indice ++;
        if(hash_iter_al_final(iter))return false;
        buscando = hash_iter_ver_actual(iter) == NULL;
    }
    return true;
}


const char *hash_iter_ver_actual(const hash_iter_t *iter){
    if((iter->hash->tabla[iter->indice].estado) != OCUPADO) return NULL;
    return iter->hash->tabla[iter->indice].clave;
}


bool hash_iter_al_final(const hash_iter_t *iter){
    if(iter->hash->cantidad == 0)return true;
    return (iter->indice  >= iter->hash->capacidad - 1);
}


void hash_iter_destruir(hash_iter_t* iter){
    free(iter);
}

