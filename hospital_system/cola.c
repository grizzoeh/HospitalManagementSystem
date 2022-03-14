#include "cola.h"
#include <stdlib.h>


/* Definición del struct nodo 
 */

typedef struct nodo {
    void *dato;
    struct nodo *siguiente;   
      
} nodo_t;

nodo_t *nodo_crear_(void){
    nodo_t *nodo = malloc(sizeof(nodo_t));
    if (nodo == NULL) {
        return NULL;
    }
    nodo->dato = NULL;
    nodo->siguiente = NULL;
    return nodo;
}
void nodo_destruir(nodo_t *nodo){
    free(nodo);
}



/* Definición del struct cola 
 */
struct cola {
    nodo_t *primero;   // siempre tengo que usar nodo_t por que es otro struct.
    nodo_t *ultimo;  
};


/* *****************************************************************
 *                    PRIMITIVAS DE LA PILA
 * *****************************************************************/

// ...
cola_t *cola_crear(void){
    cola_t *cola = malloc(sizeof(cola_t));
    if (cola == NULL) {
        return NULL;
    }

    cola->primero = NULL;
    cola->ultimo = NULL;
    return cola;
}

bool cola_esta_vacia(const cola_t *cola){
    return cola->primero == NULL;
}

void *cola_ver_primero(const cola_t *cola){
    if (cola_esta_vacia(cola)){
        return NULL;
    }
    return cola->primero->dato;
}

bool cola_encolar(cola_t *cola, void *valor){
    nodo_t *nuevo_nodo = nodo_crear_();
    if (nuevo_nodo == NULL){
        return false;
    }
    nuevo_nodo->dato = valor;

    if (cola_esta_vacia(cola)){
        cola->primero = nuevo_nodo;
       

    }else{
        cola->ultimo->siguiente = nuevo_nodo;
        
    }
    cola->ultimo = nuevo_nodo;
    return true;

}

void *cola_desencolar(cola_t *cola){
    if (cola_esta_vacia(cola)){
        
        return NULL;
    }
    nodo_t *nodo_saliente = cola->primero;
    void *nodo_saliente_dato = nodo_saliente->dato; 
    cola->primero = cola->primero->siguiente;
    nodo_destruir(nodo_saliente);
    // tambien puedo hacer free(nodo_saliente) y no usar la primitiva nodo_destruir;

    //luego de desencolar el ultimo, el puntero al último queda apuntando a basura, esto puede generar bugs
    //entonces lo cambio a NULL.
    if (cola_esta_vacia(cola)){
        cola->ultimo = NULL;
    }
    
    return nodo_saliente_dato;
       
}


void cola_destruir(cola_t *cola, void (*destruir_dato)(void *)){
    while (!cola_esta_vacia(cola)){
        if (destruir_dato == NULL){
            cola_desencolar(cola);

        }else{  
            destruir_dato(cola_desencolar(cola));   
        }
    }
    free(cola);
}

