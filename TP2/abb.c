#define _POSIX_C_SOURCE 200809L
#include "abb.h"
#include <stdlib.h>
#include <string.h>
#include "pila.h"

/* *****************************************************************
 *                DEFINICION DE LOS TIPOS DE DATOS
 * *****************************************************************/

typedef struct nodo
{
    struct nodo *izq;
    struct nodo *der;
    char *clave;
    void *dato;
} abb_nodo_t;

struct abb
{
    abb_nodo_t *raiz;
    abb_destruir_dato_t destruir_dato;
    abb_comparar_clave_t comparar_clave;
};

struct abb_iter{
    
    const abb_t* arbol;
    pila_t* pila;

};


/* *****************************************************************
 *                    PRIMITIVAS DEL ABB
 * *****************************************************************/

abb_t *abb_crear(abb_comparar_clave_t cmp, abb_destruir_dato_t destruir_dato)
{
    abb_t *arbol = malloc(sizeof(abb_t));
    if (!arbol)
        return NULL;

    arbol->comparar_clave = cmp;
    arbol->destruir_dato = destruir_dato;
    arbol->raiz = NULL;
    return arbol;
}


abb_nodo_t *crear_nodo(const char *clave, void *dato)
{
    abb_nodo_t *nodo = malloc(sizeof(abb_nodo_t));
    if (!nodo)
        return false;
    nodo->clave = strdup(clave);
    nodo->dato = dato;
    nodo->izq = NULL;
    nodo->der = NULL;
    return nodo;
}


abb_nodo_t *obtener_nodo(const abb_t *arbol ,abb_nodo_t *nodo, const char *clave){
    if (!nodo)return NULL;

    int comparacion = arbol->comparar_clave(clave, nodo->clave); 

    if (comparacion == 0)return nodo;

    if(comparacion < 0) return obtener_nodo(arbol,nodo->izq, clave);

    return obtener_nodo(arbol,nodo->der, clave);
}


bool ubicar_nodo(abb_t *arbol,abb_nodo_t *padre,abb_nodo_t *actual, const char *clave, void *dato){
    if(!actual){
        int resultado = arbol->comparar_clave(clave, padre->clave);
        if (resultado < 0){
            abb_nodo_t *nodo = crear_nodo(clave, dato);
            if (!nodo)return false;
            padre->izq = nodo;
        }else{
            abb_nodo_t *nodo = crear_nodo(clave, dato);
            if (!nodo)return false;
            padre->der = nodo;
        }
        return true;
    }

    int resultado = arbol->comparar_clave(clave, actual->clave);

    if(resultado == 0){
        if(arbol->destruir_dato)arbol->destruir_dato(actual->dato);
        actual->dato = dato;
        return true;
    }

    if (resultado < 0){
        return ubicar_nodo(arbol, actual, actual->izq, clave, dato);
    }else{
        return ubicar_nodo(arbol, actual, actual->der, clave, dato);
    }
}


bool abb_guardar(abb_t *arbol, const char *clave, void *dato){
    if (!arbol->raiz){
        abb_nodo_t *nodo = crear_nodo(clave, dato);
        if (!nodo)return false;
        arbol->raiz = nodo;
        return true;
    }
    return ubicar_nodo(arbol,NULL, arbol->raiz, clave, dato);
}


int calcular_hijos(abb_nodo_t *nodo)
{
    int cantidad = 0;
    if (nodo->izq != NULL)
        cantidad++;
    if (nodo->der != NULL)
        cantidad++;
    return cantidad;
}


void borrar_0_1(abb_nodo_t *nodo_actual, abb_nodo_t *nodo_padre, abb_t *arbol, char dir){
    abb_nodo_t *nodo = nodo_actual->izq;
    if (nodo == NULL)nodo = nodo_actual->der;

    if (dir == 'i')nodo_padre->izq = nodo;
    if (dir == 'd')nodo_padre->der = nodo;

    if(nodo_actual == arbol->raiz)arbol->raiz = nodo;

    free(nodo_actual->clave);
    free(nodo_actual);
}


void borrar_2(abb_nodo_t *nodo_actual, abb_nodo_t *nodo_padre, abb_t *arbol, char dir){
    abb_nodo_t *reemplazante = nodo_actual->izq;
    while (reemplazante->der != NULL)reemplazante = reemplazante->der;

    char *aux_clave = strdup(reemplazante->clave);
    void *aux_dato = reemplazante->dato;

    abb_borrar(arbol, reemplazante->clave);

    free(nodo_actual->clave);
    nodo_actual->clave = aux_clave;
    nodo_actual->dato = aux_dato;
}


void *borrar(abb_nodo_t *nodo_actual, abb_nodo_t *nodo_padre, const char *clave, abb_t *arbol){
    if (nodo_actual == NULL)return NULL;

    int resultado = arbol->comparar_clave(clave, nodo_actual->clave);

    if (resultado == 0){
        void *aux_dato = nodo_actual->dato;
        char dir;
        if(nodo_actual != arbol->raiz){
            resultado = arbol->comparar_clave(clave, nodo_padre->clave);
            dir = (resultado < 0 ) ? 'i': 'd';
        }else{
            resultado = 0;
            dir = 'N';
        }
        int hijos = calcular_hijos(nodo_actual);
        if(hijos == 2){
            borrar_2(nodo_actual, nodo_padre, arbol,dir);
        }else{
            borrar_0_1(nodo_actual, nodo_padre, arbol, dir);
        }
        return aux_dato;
    }
    if (resultado < 0)return borrar(nodo_actual->izq, nodo_actual, clave, arbol);

    if (resultado > 0)return borrar(nodo_actual->der, nodo_actual, clave, arbol);

    return NULL;
}


void *abb_borrar(abb_t *arbol, const char *clave){
    return borrar(arbol->raiz, NULL, clave, arbol);
}


void *abb_obtener(const abb_t *arbol, const char *clave){
    abb_nodo_t *nodo = obtener_nodo(arbol, arbol->raiz, clave);
    if(!nodo)return NULL;

    return nodo->dato;
}


bool abb_pertenece(const abb_t *arbol, const char *clave){
    return (obtener_nodo(arbol, arbol->raiz, clave) != NULL);
}


int contar_nodos(abb_nodo_t *nodo){
    if (!nodo)return 0;

    int actual = 1;

    int cant_izq = contar_nodos(nodo->izq);
    int cant_der = contar_nodos(nodo->der);

    return (actual + cant_izq + cant_der);
}


size_t abb_cantidad(const abb_t *arbol){
    return contar_nodos(arbol->raiz);
}


void destruccion(abb_t *arbol, abb_nodo_t *nodo){
    if (nodo == NULL)return;

    destruccion(arbol, nodo->izq);
    destruccion(arbol, nodo->der);

    free(nodo->clave);
    if(arbol->destruir_dato)arbol->destruir_dato(nodo->dato);
    free(nodo);
}


void abb_destruir(abb_t *arbol){
    destruccion(arbol, arbol->raiz);
    free(arbol);
}


/* *****************************************************************
 *                    AUXILIARES DEL ITERADOR
 * *****************************************************************/
 void pila_apilar_iterador(pila_t* pila, abb_nodo_t* nodo){

     if(!pila){
         return ;
     } 

     if(!nodo){
         return;
     } 

     pila_apilar(pila, nodo);
     pila_apilar_iterador(pila, nodo->izq);

 }



/* *****************************************************************
 *                    PRIMITIVAS DEL ITERADOR INTERNO
 * *****************************************************************/

bool abb_iterar( bool visitar(const char *, void *, void *), void *extra, abb_nodo_t* nodo_actual){
   
    if(!nodo_actual) return true;

    if(!abb_iterar(visitar, extra, nodo_actual->izq))return false;

    if(!visitar(nodo_actual->clave, nodo_actual->dato, extra))return false;

    if(!abb_iterar(visitar, extra, nodo_actual->der))return false;
    
    return true;
}

void abb_in_order(abb_t *arbol, bool visitar(const char *, void *, void *), void *extra){

    abb_iterar(visitar, extra, arbol->raiz);
}


 /* *****************************************************************
 *                    PRIMITIVAS DEL ITERADOR EXTERNO
 * *****************************************************************/


abb_iter_t *abb_iter_in_crear(const abb_t *arbol){

    abb_iter_t* iter = malloc(sizeof(abb_iter_t));
    if(!iter){
        return NULL;
    }

    pila_t* pila = pila_crear();
    if(!pila){
        free(iter);
        return NULL;
    }

    iter->pila = pila;
    iter->arbol = arbol;
    pila_apilar_iterador(pila, arbol->raiz);

    return iter;
}

bool abb_iter_in_al_final(const abb_iter_t *iter){

    return pila_esta_vacia(iter->pila);
}

bool abb_iter_in_avanzar(abb_iter_t *iter){

    if(abb_iter_in_al_final(iter)){
        return false;
    }

    abb_nodo_t* nodo_actual = pila_desapilar(iter->pila);
    pila_apilar_iterador(iter->pila, nodo_actual->der);

    return true;
}

const char *abb_iter_in_ver_actual(const abb_iter_t *iter){

    if(abb_iter_in_al_final(iter)){
        return NULL;
    }

    abb_nodo_t* nodo_actual = pila_ver_tope(iter->pila);
    return nodo_actual->clave;
}

void abb_iter_in_destruir(abb_iter_t* iter){
    pila_destruir(iter->pila);
    free(iter);
}
