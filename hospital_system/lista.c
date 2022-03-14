#include "lista.h"
#include <stdlib.h>


/* Definición del struct nodo 
 */

typedef struct nodo {
    void *dato;
    struct nodo *siguiente;   
      
} nodo_t;

//primitiva nodo
nodo_t *nodo_crear(void){
    nodo_t *nodo = malloc(sizeof(nodo_t));
    if (nodo == NULL) {
        return NULL;
    }
    nodo->dato = NULL;
    nodo->siguiente = NULL;
    return nodo;
}

/* Definición del struct lista 
 */
struct lista {
    nodo_t *primero;   // siempre tengo que usar nodo_t por que es otro struct.
    nodo_t *ultimo;
    size_t largo;  
};

/* Definición del struct iterador 
 */

struct lista_iter{
    lista_t *lista;
    nodo_t *actual;
	nodo_t *anterior;
};


/* *****************************************************************
 *                    PRIMITIVAS DE LA LISTA
 * *****************************************************************/

// ...
lista_t *lista_crear(void){
    lista_t *lista = malloc(sizeof(lista_t));
    if (!lista) {
        return NULL;
    }

    lista->primero = NULL;
    lista->ultimo = NULL;
    lista->largo = 0;
    return lista;
}

bool lista_esta_vacia(const lista_t *lista){
    return lista->largo == 0;
}

void *lista_ver_primero(const lista_t *lista){
    if (lista_esta_vacia(lista)){
        return NULL;
    }
    return lista->primero->dato;
}

void *lista_ver_ultimo(const lista_t *lista){
    if (lista_esta_vacia(lista)){
        return NULL;
    }

    return lista->ultimo->dato;
}

size_t lista_largo(const lista_t *lista){
    return lista->largo;
}

bool lista_insertar_primero(lista_t *lista, void *dato){
    nodo_t *nuevo_nodo = nodo_crear();
    if (nuevo_nodo == NULL){
        return false;
    }
    nuevo_nodo->dato = dato;
    nuevo_nodo->siguiente = lista->primero;

    if (lista_esta_vacia(lista)){
        //si esta vacia y agrego uno al principio, el ultimo debe ser igual que el primero
        lista->ultimo = nuevo_nodo;
    }
       
    lista->primero = nuevo_nodo;
    lista->largo++;
    return true;

}


bool lista_insertar_ultimo(lista_t *lista, void *dato){
    nodo_t *nuevo_nodo = nodo_crear();
    if (!nuevo_nodo){
        return false;
    }
    nuevo_nodo->dato = dato;
    

    if (!lista_esta_vacia(lista)) {
        lista->ultimo->siguiente=nuevo_nodo;
    }else{
        lista->primero=nuevo_nodo;
    }
    lista->ultimo = nuevo_nodo;
    lista->largo++;
    return true;
}


void *lista_borrar_primero(lista_t *lista){
    if (lista_esta_vacia(lista)){
        return NULL;
    }

    nodo_t *nodo_saliente = lista->primero;
    void *nodo_saliente_dato = nodo_saliente->dato; 
    

    if (!lista->primero->siguiente){
        lista->ultimo = NULL;
    }

    lista->primero = lista->primero->siguiente;
    free(nodo_saliente);
    
    lista->largo--;
    return nodo_saliente_dato;

}


void lista_destruir(lista_t *lista, void (*destruir_dato)(void *)){
    while (!lista_esta_vacia(lista)){
        if (destruir_dato == NULL){
            lista_borrar_primero(lista);

        }else{  
            destruir_dato(lista_borrar_primero(lista));   
        }
    }
    free(lista);
}


//iterador interno
void lista_iterar(lista_t* lista, bool visitar(void* dato, void* extra), void* extra){
    
    nodo_t *nodo = lista->primero;
    //recordar que la funcion visitar es bool que devuelve false como condicion de corte indicando
    //que no hay que iterar mas, si da true hay que seguir iterando.
    bool visitar_estado = true;
    while(nodo && visitar_estado){
        visitar_estado = visitar(nodo->dato,extra);
        nodo = nodo->siguiente;
    }
}




/* *****************************************************************
 *                    PRIMITIVAS DEL ITERADOR
 * *****************************************************************/

// ...

lista_iter_t *lista_iter_crear(lista_t *lista){
    lista_iter_t *iter = malloc(sizeof(lista_iter_t));
    if (iter == NULL) {
        return NULL;
    }
    iter->lista = lista;
    iter->actual = lista->primero;
    iter->anterior = NULL;
    return iter;
}

void *lista_iter_ver_actual(const lista_iter_t *iter){

	if (lista_iter_al_final(iter)){
        return NULL;
    }

 	return iter->actual->dato;
}

bool lista_iter_al_final(const lista_iter_t *iter){
    return iter->actual == NULL;
}

bool lista_iter_avanzar(lista_iter_t *iter){
	if (lista_iter_al_final(iter)){
        return false;
    } 

	iter->anterior = iter->actual;
	iter->actual = iter->actual->siguiente;
	return true;
}


void lista_iter_destruir(lista_iter_t *iter){
	free(iter);
}

bool lista_iter_insertar(lista_iter_t *iter, void *dato){
    nodo_t* nodo_a_insertar = nodo_crear();
    nodo_a_insertar->dato = dato;

    if(!nodo_a_insertar){ 
        return false;
    }

    
    if(!iter->anterior && iter->actual){
        iter->lista->primero = nodo_a_insertar;
    }else if (lista_esta_vacia(iter->lista)){
        iter->lista->primero = nodo_a_insertar;
        iter->lista->ultimo = nodo_a_insertar;
    } else if(lista_iter_al_final(iter)){  
        iter->lista->ultimo = nodo_a_insertar;
        iter->anterior->siguiente = nodo_a_insertar;
    } else if(iter->anterior && iter->actual){
        iter->anterior->siguiente = nodo_a_insertar;

    }
    nodo_a_insertar->siguiente = iter->actual;
    iter->actual = nodo_a_insertar;
    iter->lista->largo++;
    return true;
}

void *lista_iter_borrar(lista_iter_t *iter){

    if(lista_esta_vacia(iter->lista) || lista_iter_al_final(iter)){
        return NULL;
    }

    
    if(!iter->anterior){
        iter->lista->primero = iter->actual->siguiente;        
    }

    
    if(!iter->actual->siguiente){
        iter->lista->ultimo = iter->anterior;        
    }

    
    if(iter->anterior){
        iter->anterior->siguiente = iter->actual->siguiente;
    }

    nodo_t* a_eliminar = iter->actual;
    void* dato_a_eliminar = a_eliminar->dato;
    iter->actual = a_eliminar->siguiente;
    free(a_eliminar);
    iter->lista->largo--;
    return dato_a_eliminar;


}
