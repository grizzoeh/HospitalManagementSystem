#include "pila.h"
#include <stdlib.h>

#define TAM 10
#define AUMENTAR_CAPACIDAD_EN (pila->capacidad)*
#define REDUCIR_CAPACIDAD_EN (pila->capacidad)/

/* Definición del struct pila proporcionado por la cátedra.
 */
struct pila {
    void **datos;
    size_t cantidad;   // Cantidad de elementos almacenados.
    size_t capacidad;  // Capacidad del arreglo 'datos'.
};

//funciones auxiliares
// Ajusta la dimension de la pila
// Pre: la pila fue creada.
// Post: la pila cambió de capacidad a nueva_cap y devuelve true
// o la pila queda intacta y devuelve false si no se pudo cambiar la capacidad
// a nueva_cap
bool pila_redimensionar(pila_t *pila, size_t nueva_cap) {
    void **datos_nuevo = realloc(pila->datos, nueva_cap * sizeof(void*));

    
    //fallo del realloc.
    if (datos_nuevo == NULL) {
        return false;
    }

    pila->datos = datos_nuevo;
    pila->capacidad = nueva_cap;
    return true;
}

/* *****************************************************************
 *                    PRIMITIVAS DE LA PILA
 * *****************************************************************/

// ...
pila_t *pila_crear(void){
    pila_t *pila = malloc(sizeof(pila_t));
    if (pila == NULL) {
        return NULL;
    }
    pila->datos = malloc(TAM*sizeof(void*));

    if (pila->datos == NULL) {
        free(pila);
        return NULL;
    }
    //Inicializo los atributos de la pila como si fuera vacia (cantidad de elementos 0
    // y tamaño prefijado)
    pila->capacidad = TAM;
    pila->cantidad = 0;
    return pila;
}

void pila_destruir(pila_t *pila){
    free(pila->datos);
    free(pila);
}

bool pila_esta_vacia(const pila_t *pila){

    return pila->cantidad == 0;

}

bool pila_apilar(pila_t *pila, void *valor){

    if (pila->cantidad == pila->capacidad){

        if (!pila_redimensionar(pila,AUMENTAR_CAPACIDAD_EN 2)){
            return false;
            //si dimensionar no devuelve false, redimensiona aumentando la
            //capacidad al doble
        }
        
    }
     
    pila->datos[pila->cantidad] = valor;
    pila->cantidad++;
    return true;

    
}

void *pila_ver_tope(const pila_t *pila){
    if (pila_esta_vacia(pila)){
        return NULL;
    }
    return pila->datos[pila->cantidad-1];

}

void *pila_desapilar(pila_t *pila){
    if (pila_esta_vacia(pila)){
        return NULL;
    }
    // simplemente resto 1 a cantidad, el valor b pasara a ser "basura de C"
    pila->cantidad--;

    if (AUMENTAR_CAPACIDAD_EN 4 <= pila->capacidad && pila->capacidad>=TAM*2){

        pila_redimensionar(pila,REDUCIR_CAPACIDAD_EN 2);
       
    }
    
    return pila->datos[pila->cantidad];
}
