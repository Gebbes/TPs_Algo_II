#include "tp0.h"
#include <stdio.h>

/* *****************************************************************
 *                     FUNCIONES A COMPLETAR                       *
 *         (ver en tp0.h la documentación de cada función)         *
 * *****************************************************************/

void swap(int *x, int *y) {
    int aux = *x;
    *x = *y ;
    *y = aux; 
}


int maximo(int vector[], int n) {
    int posicion;
    if(n == 0) 
    {
        return -1;
    }
    posicion = 0;
    for(int i = 1; i < n; i++)
    {
        if( vector[posicion] < vector[i])
        {
            posicion = i;
        }
    }
    return posicion;
}


int comparar(int vector1[], int n1, int vector2[], int n2) {
    int respuesta = 0;          // Valor que tomara el return
    int encontrado = 0;   // Este 0 representaria al booleano False, y 1 al True
    
    for(int i=0; i < n1 && i < n2; i++){
        if( vector1[i] > vector2[i] ){
            return 1;
        }
        if( vector1[i] < vector2[i]){
            return -1;
        }
    }
    if(encontrado == 0 && n1<n2){
        return -1;
    }
    if(encontrado == 0 && n2<n1){
        return 1;
    }
    return respuesta;
}


void seleccion(int vector[], int n) {
    int m = n - 1 ;         // Para mayor legibilidad, marco la ultima posicion
    for(int i = 0; i <= m; i++)
        {
            int posicion_max ;
            posicion_max = maximo(vector, (n-i)) ;
            swap(&vector[m-i], &vector[posicion_max]);
        }
}

