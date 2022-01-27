#include "heap.h"
#include <stdlib.h>
#include <stdio.h>   //BORRAR
#define CAPACIDAD_ESTANDARD 15
#define K_REDIMENSION 2

struct heap
{
    void **datos;
    size_t cantidad;
    size_t capacidad;
    cmp_func_t comparacion;
};

/* *****************************************************************
 *                    FUNCIONES AUXILIARES
 * *****************************************************************/

bool redimension( heap_t *heap, size_t nueva_capacidad)
{
    void **aux_datos = realloc(heap->datos,sizeof(void*) *nueva_capacidad);
    if(aux_datos == NULL)return false;
    if(heap->datos == NULL)return false;

    heap->datos = aux_datos;
    heap->capacidad = nueva_capacidad;
    return true;
}

void swap(void** x, void** y) {
    void* valor_intercambio = *x;
    *x = *y; 
    *y = valor_intercambio;
}

size_t posicion_padre(size_t pos_hijo){
    return (pos_hijo - 1)/2;
}

size_t posicion_hijo_izq(size_t pos_padre){
    return 2* pos_padre + 1;
}

size_t posicion_hijo_der(size_t pos_padre){
    return 2* pos_padre + 2;
}

void upheap(void** datos, size_t pos_hijo, cmp_func_t cmp){

    if(pos_hijo <= 0){
        return;
    }

    size_t pos_padre = posicion_padre(pos_hijo);

    if(cmp(datos[pos_padre], datos[pos_hijo]) >= 0){
        return;
    }

    swap(&datos[pos_padre], &datos[pos_hijo]);
    upheap(datos, pos_padre, cmp);
}

void downheap( void** datos, size_t pos_padre, size_t cant, cmp_func_t cmp){

    size_t pos_hijo_izq = posicion_hijo_izq(pos_padre);

    if(cant <= pos_hijo_izq){
        return;
    }

    size_t pos_hijo_der = posicion_hijo_der(pos_padre);
    size_t pos_max = pos_padre;

    if(cmp(datos[pos_hijo_izq], datos[pos_padre]) > 0){
        pos_max = pos_hijo_izq;
    }

    if(pos_hijo_der < cant){
        if(cmp(datos[pos_hijo_der], datos[pos_max]) > 0){
            pos_max = pos_hijo_der;
        }
    }

    if(pos_max == pos_padre){
        return;
    }

    swap(&datos[pos_padre], &datos[pos_max]);
    downheap(datos, pos_max, cant, cmp);

}

void heapify(void *arreglo[], size_t n, cmp_func_t cmp){
    for(size_t i = n; i > 0; i--){
        downheap(arreglo, i - 1, n, cmp);
    }
}

/* *****************************************************************
 *                    PRIMITIVAS DEL HEAP
 * *****************************************************************/

heap_t *heap_crear(cmp_func_t cmp)
{
    heap_t *heap = malloc(sizeof(heap_t));
    if(!heap) return NULL;

    heap->cantidad = 0;
    heap->capacidad = CAPACIDAD_ESTANDARD;
    heap->datos = malloc(sizeof(void *) * CAPACIDAD_ESTANDARD);
    heap->comparacion = cmp;
    return heap;
}

heap_t *heap_crear_arr(void *arreglo[], size_t n, cmp_func_t cmp)
{
    heap_t *heap = heap_crear(cmp);
    
    if(!heap){
        free(heap);
        return NULL;
    }

    if(n *K_REDIMENSION >= heap->capacidad && !redimension(heap, n * K_REDIMENSION)){
        return NULL;
    }

    for(size_t i = 0; i < n; i++)
    {
        heap->datos[i] = arreglo[i];
    }

    heap->cantidad = n;

    heapify(heap->datos, heap->cantidad, cmp);

    return heap;
}


void heap_destruir(heap_t *heap, void destruir_elemento(void *e))
{
    if(destruir_elemento)
    {
        for(size_t i = 0; i < heap->cantidad; i++)
        {
        destruir_elemento(heap->datos[i]);
        }
    }

    free(heap->datos);
    free(heap);
}

size_t heap_cantidad(const heap_t *heap)
{
    return heap->cantidad;
}


bool heap_esta_vacio(const heap_t *heap)
{
    return (heap_cantidad(heap) == 0);
}

bool heap_encolar(heap_t *heap, void *elem){
    if(heap->cantidad == heap->capacidad && !redimension(heap,heap->capacidad * K_REDIMENSION)){
        return false;
    }

    heap->datos[heap->cantidad] = elem;
    upheap(heap->datos, heap->cantidad, heap->comparacion);
    heap->cantidad++;
    
    return true;
}

void *heap_ver_max(const heap_t *heap){

    if(heap_esta_vacio(heap)){
        return NULL;
    }

    return heap->datos[0];
}

void *heap_desencolar(heap_t *heap){

     if(heap_esta_vacio(heap)){
        return NULL;
    }

    void* dato = heap_ver_max(heap);
    swap(&heap->datos[0], &heap->datos[heap->cantidad -1]);
    

    if(heap->cantidad == (heap->capacidad /K_REDIMENSION)){
        if(!redimension(heap, heap->capacidad / K_REDIMENSION)){
            return NULL;
        }
    }

    heap->cantidad--;
    downheap(heap->datos, 0, heap->cantidad, heap->comparacion);

    return dato;
}

/* *****************************************************************
 *                    HEAP SORT
 * *****************************************************************/

void heap_sort(void *elementos[], size_t cant, cmp_func_t cmp){

    heapify(elementos, cant, cmp);

    for(size_t i = 1; i < cant; i++){
        swap(&elementos[0], &elementos[cant - i]);
        downheap(elementos, 0, cant - i, cmp);
    }
}


