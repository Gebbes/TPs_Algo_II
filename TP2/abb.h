#ifndef ABB_H
#define ABB_H

#include <stdbool.h>
#include <stddef.h>

/* *****************************************************************
 *                DEFINICION DE LOS TIPOS DE DATOS
 * *****************************************************************/

typedef struct abb abb_t;

typedef int (*abb_comparar_clave_t) (const char *, const char *);

typedef void (*abb_destruir_dato_t) (void *);

typedef struct abb_iter abb_iter_t;


/* *****************************************************************
 *                    PRIMITIVAS DEL ABB
 * *****************************************************************/

// Crea el arbol
// Pre: Recibe una funcion de comparacion no nula, y recibe una de destruccion
// que puede ser NULL, ambas deben cumplir las firmas especificadas en "TIPOS DE DATOS"
// Post: El arbol fue creado 
abb_t* abb_crear(abb_comparar_clave_t cmp, abb_destruir_dato_t destruir_dato);

// Guarda un elemento nuevo en el arbol, si la clave ya existia en el arbol, reemplaza el valor
// y elimina el anterior con la funcion "destruir_dato" pedida.
// Pre: El arbol fue creado.
// Post: Devuelve True en caso de haber podido guardar el elemento, de lo contrario false.
bool abb_guardar(abb_t *arbol, const char *clave, void *dato);

// Borra el elemento y su clave del arbol. Devuelve el elemento, y de no existir la clave
// devuelve NULL.
// Pre: El arbol fue creado.
void *abb_borrar(abb_t *arbol, const char *clave);

// Dada una clave, devuelve su dato asociado. Si no existe en el arbol, devuelve NULL.
// Pre: El arbol fue creado.
void *abb_obtener(const abb_t *arbol, const char *clave);

// Dada una clave, devuelve true si existe en el arbol, de lo contrario false.
// Pre: El arbol fue creado.
bool abb_pertenece(const abb_t *arbol, const char *clave);

// Devuelve la cantidad de elementos en el arbol.
// El arbol fue creado.
size_t abb_cantidad(const abb_t *arbol);

// Destruye el arbol, eliminando todos los elementos con la funcion "destruir_dato" pedida.
// Pre: El arbol fue creado.
void abb_destruir(abb_t *arbol);


/* *****************************************************************
 *                    PRIMITIVAS DEL ITERADOR INTERNO
 * *****************************************************************/

// Itera el arbol in-order, usa la funcion visitar que recibe la clave, el valor y un puntero extra, si devuelve false debe dejar de iterar
// Pre: El arbol fue creado
void abb_in_order(abb_t *arbol, bool visitar(const char *, void *, void *), void *extra);


 /* *****************************************************************
 *                    PRIMITIVAS DEL ITERADOR EXTERNO
 * *****************************************************************/

// Crear un iterador externo
// Devuelve el iterador o NULL si no pudo crearse
// Pre: El arbol fue creado
abb_iter_t *abb_iter_in_crear(const abb_t *arbol);

// Avanza a la siguiente posision del iterador, de forma in-order 
// Devuelve true si pudo avanzar a la siguiente posision o false en casa contrario
// Pre: El iterador fue creado
bool abb_iter_in_avanzar(abb_iter_t *iter);

// Devuelve la clave de la posicion donde se encuentra el iterador
// Pre: El iterador fue creado
const char *abb_iter_in_ver_actual(const abb_iter_t *iter);

// Devuelve true si es la ultima posision del iterador o false en casa contrario
// Pre: El iterador fue creado
bool abb_iter_in_al_final(const abb_iter_t *iter);

// Destruye el iterador
// Pre: El iterador fue creado
void abb_iter_in_destruir(abb_iter_t* iter);


#endif  // ABB_H