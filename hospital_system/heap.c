#include "heap.h" 
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h> 
#include <string.h> 

#define TAMANIO_INICIAL 20 
#define FACTOR_REDUCCION 0.25 
#define FACTOR_REDIMENSION 2 


//Cabe aclarar que este heap es un heap de Màximos =)


/*
 * Implementación de un TAD cola de prioridad, usando un max-heap.
 *
 * Notar que al ser un max-heap el elemento mas grande será el de mejor
 * prioridad. Si se desea un min-heap, alcanza con invertir la función de
 * comparación.
 */

/* Tipo utilizado para el heap. */
typedef struct heap{
	void ** elementos;
	cmp_func_t comparador;
	size_t tamanio;
	size_t cantidad_total;
}heap_t;

/* Crea un heap. Recibe como único parámetro la función de comparación a
 * utilizar. Devuelve un puntero al heap, el cual debe ser destruido con
 * heap_destruir().
 */
heap_t *heap_crear(cmp_func_t cmp){
	if (cmp == NULL){
		return NULL;
	}
	heap_t * heap = malloc(sizeof(heap_t));
	if (heap){
		heap->elementos = calloc(TAMANIO_INICIAL, sizeof(void*));
		if (!heap->elementos){
			free(heap);
			return NULL;
		}
		heap->comparador = cmp;
		heap->cantidad_total = 0;
		heap->tamanio = TAMANIO_INICIAL;
	}
	return heap;
}

/*
 * Esta funcion intercambia de lugar dos valores
 * Esta funcion es practicamente identica a la del TP0
 */
void intercambio(void ** elementos, size_t primero, size_t segundo) {
	void * auxiliar = elementos[primero];
	elementos[primero] = elementos[segundo];
	elementos[segundo] = auxiliar; 
}


/*
	Recibe, el vector de elementos, la cantidad, la funcion de comparacion, la posicion actual y las de su izquierda y derecha para asi comparar.
	Devuelve la posicion que es mayor a la actual,
*/
size_t posicion_mayor(void **elementos, size_t cantidad, cmp_func_t cmp, size_t posicion_act, size_t pos_izq, size_t pos_der) {
    size_t posicion_maxima = 0;
    posicion_maxima = posicion_act;
    if (pos_izq != -1){ 
 	   if(pos_izq < cantidad && cmp(elementos[posicion_maxima], elementos[pos_izq]) < 0){
			posicion_maxima = pos_izq;
    	}
	}
	if (pos_der != -1){
    	if(pos_der < cantidad && cmp(elementos[posicion_maxima], elementos[pos_der]) < 0){
			posicion_maxima = pos_der;	
		}
	}

    return posicion_maxima;
}

/*
 *Funcion que aplica downheap al vector de elementos
 * Pre: Recibe un vector de elementos, la cantidad de elementos, la posicion actual 
 * y una funcion de comparacion de elementos previamente vàlidos
 *
 */
void downheapear(void ** elementos, cmp_func_t cmp, size_t cantidad_elementos, size_t posicion_act){
	if (posicion_act >= cantidad_elementos){
        return;
	}
	size_t pos_hijo_der  = (2 * posicion_act) + 2;
	size_t pos_hijo_izq = (2 * posicion_act) + 1;
	if (pos_hijo_izq > cantidad_elementos ){
		pos_hijo_izq = -1;
	}	
	if (pos_hijo_der > cantidad_elementos ){
		pos_hijo_der = -1;
	}
	size_t mayor_posicion  = posicion_mayor(elementos, cantidad_elementos, cmp, posicion_act, pos_hijo_izq, pos_hijo_der);

    if(mayor_posicion != posicion_act){
        intercambio(elementos, posicion_act, mayor_posicion);
        downheapear(elementos, cmp, cantidad_elementos, mayor_posicion);
    }
}


/* Funcion que garantiza propiedad de heap */
void heapify(void** elementos, size_t cantidad_elementos, cmp_func_t cmp){

	size_t cant = cantidad_elementos/2;
    for(int i = (int)cant; i >= 0; i--){
        downheapear(elementos, cmp, cantidad_elementos, (size_t)i);
    }	

}

/* Elimina el heap, llamando a la función dada para cada elemento del mismo.
 * El puntero a la función puede ser NULL, en cuyo caso no se llamará.
 * Post: se llamó a la función indicada con cada elemento del heap. El heap
 * dejó de ser válido. */
void heap_destruir(heap_t *heap, void (*destruir_elemento)(void *e)){	

	if (destruir_elemento != NULL){
		size_t i = 0;
		while(!heap_esta_vacio(heap)){
			destruir_elemento(heap->elementos[i]);
			heap->cantidad_total--;
			i++;
		}
	}
	free(heap->elementos);
	free(heap);
}

/* Devuelve la cantidad de elementos que hay en el heap. */
size_t heap_cantidad(const heap_t *heap){
	return heap->cantidad_total;
}
/* Devuelve true si la cantidad de elementos que hay en el heap es 0, false en
 * caso contrario. 
 */
bool heap_esta_vacio(const heap_t *heap){
	return(heap->cantidad_total == 0);
}



/*
 *Funcion que aplica upheap al vector de elementos
 * Pre: Recibe un vector de elementos, la posicion actual 
 * y una funcion de comparacion de elementos previamente vàlidos
 */
void upheapear(void ** elementos, size_t posicion_act, cmp_func_t cmp){
	  
    if (posicion_act == 0){
        return;
	}

	size_t posicion_padre = (posicion_act - 1) / 2;
    
    if (cmp(elementos[posicion_act], elementos[posicion_padre]) > 0) {
        intercambio(elementos, posicion_act, posicion_padre);
        upheapear(elementos, posicion_padre, cmp);
    }


}



/*
 *Devuelve verdadero si hay que redimensionar a un mayor tamaño el heap
 */
bool hay_que_agrandar(const heap_t * heap){
	return (heap->cantidad_total  == heap->tamanio);
}

/*
 * Devuelve verdadero si hay que redimensionar a un menor tamaño el heap
 */
bool hay_que_achicar(const heap_t * heap){
	return ((heap->cantidad_total < ((double)heap->tamanio * FACTOR_REDUCCION)) && heap->tamanio > TAMANIO_INICIAL);

}

/*
 * Recibe un heap y el proximo tamaño a redimensionar
 * Actualiza el puntero a un nuevo vector de elementos con el proximo tamaño
 */
int redimensionar(heap_t * heap, size_t nuevo_tamanio){
	//printf("NUEVO %i \n", nuevo_tamanio);
	void * datos = realloc(heap->elementos, nuevo_tamanio * sizeof(void *));
	if (datos == NULL){
		return -1;
	}
	heap->tamanio = nuevo_tamanio;
	heap->elementos = datos;
	return 0;
}

/* Agrega un elemento al heap. El elemento no puede ser NULL.
 * Devuelve true si fue una operación exitosa, o false en caso de error.
 * Pre: el heap fue creado.
 * Post: se agregó un nuevo elemento al heap.
 */
bool heap_encolar(heap_t *heap, void *elem){
	if(hay_que_agrandar(heap)){
		int agrandado = redimensionar(heap, (heap->tamanio * FACTOR_REDIMENSION));
		if (agrandado == -1){
			return false;
		}		
	}
	heap->elementos[heap->cantidad_total] = elem;
	heap->cantidad_total++;
	if (heap->cantidad_total != 1){
		upheapear(heap->elementos, heap->cantidad_total - 1, heap->comparador);
	}
	return true;
}

/* Devuelve el elemento con máxima prioridad. Si el heap esta vacío, devuelve
 * NULL.
 * Pre: el heap fue creado.
 */
void *heap_ver_max(const heap_t *heap){
	if (heap_esta_vacio(heap)){
		return NULL;
	}
	return heap->elementos[0];
}


/* Elimina el elemento con máxima prioridad, y lo devuelve.
 * Si el heap esta vacío, devuelve NULL.
 * Pre: el heap fue creado.
 * Post: el elemento desencolado ya no se encuentra en el heap.
 */
void *heap_desencolar(heap_t *heap){
	if(heap_esta_vacio(heap)){
		return NULL;
	}
	void * dato = heap->elementos[0];
	heap->cantidad_total--;
	if (!heap_esta_vacio(heap)){
		if (heap->cantidad_total == 1){
			heap->elementos[0] = heap->elementos[heap->cantidad_total];
		}
		else{
			intercambio(heap->elementos, 0, heap->cantidad_total );
			downheapear(heap->elementos, heap->comparador, heap->cantidad_total , 0);
		}
	}
	if (hay_que_achicar(heap)){
		int reducido = redimensionar(heap, heap->tamanio / FACTOR_REDIMENSION);
		if(reducido == -1){
			return NULL;
		}
	}
	return dato;
}

/*
 * Constructor alternativo del heap. Además de la función de comparación,
 * recibe un arreglo de valores con que inicializar el heap. Complejidad
 * O(n).
 *
 * Excepto por la complejidad, es equivalente a crear un heap vacío y encolar
 * los valores de uno en uno
 */
heap_t *heap_crear_arr(void *arreglo[], size_t n, cmp_func_t cmp){
		if (cmp == NULL){
		return NULL;
	}
	heap_t * heap = malloc(sizeof(heap_t));
	if (heap){
		heap->elementos = calloc(n, sizeof(void*));
		if (!heap->elementos){
			free(heap);
			return NULL;
		}

		memcpy(heap->elementos, arreglo, n* sizeof(void*));
		heapify(heap->elementos, n, cmp);
		heap->cantidad_total = n;
		heap->tamanio = n;
		heap->comparador = cmp;
	}
	return heap;
}


// -----------------------------------------------------------------------------//


/* Función de heapsort genérica. Esta función ordena mediante heap_sort
 * un arreglo de punteros opacos, para lo cual requiere que se
 * le pase una función de comparación. Modifica el arreglo "in-place".
 * Nótese que esta función NO es formalmente parte del TAD Heap.
 */
void heap_sort(void *elementos[], size_t cant, cmp_func_t cmp){
	if (!elementos || cant == 0)
        return;
	//primero siempre convierto el arreglo en heap
	heapify(elementos,cant,cmp);

    for (size_t i = cant - 1; i > 0; i--) {
		intercambio(elementos, 0, i);
		downheapear(elementos,cmp, i, 0);
    }

}


