#include "lista.h"
#include "hash.h"
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#define TAMANIO_INICIAL 17
#define FACTOR_DE_CARGA 3
#define FACTOR_DE_REDUCCION 0.25



typedef struct nodo_hash {
	void* dato;
	char *clave;
} nodo_hash_t;

typedef struct nodo{
	struct nodo* siguiente;
	void * dato;
}nodo_t;



struct hash_iter{
  const hash_t* hash;
  lista_iter_t* lista_iter;
  size_t posicion;
};

struct hash{
	lista_t ** tabla;
	size_t cantidad_elementos;
	size_t tamanio;
	hash_destruir_dato_t destructor;
	size_t factor_redimension;
};

typedef struct hash_campo {
    char *clave;
    void *valor;
} hash_campo_t;

struct lista_iter{
    lista_t *lista;
    nodo_t *actual;
	nodo_t *anterior;
};


//=======================(PRIMITIVAS HASH)==========================//


 // Djb2 hash function
size_t funcion_hash(const char *str) {

        unsigned long hash = 5381;
        int  c;
        while ((c = *str++))
            hash = ((hash << 5) + hash) + (unsigned long)c; /* hash * 33 + c */
        return hash;

}


hash_t *hash_crear(hash_destruir_dato_t destruir_dato){
	hash_t * hash = malloc(sizeof(hash_t));
	if (!hash){
		return NULL;
	}

	hash->tabla=calloc(TAMANIO_INICIAL, sizeof(lista_t*));
	if (!hash->tabla){
		free(hash);
		return NULL;
	}
	hash->destructor = destruir_dato;
	hash->tamanio = TAMANIO_INICIAL;
	hash->cantidad_elementos = 0;
	hash->factor_redimension = 0;
	return hash;
}


// Recibe un hash entero
// Destruye todas sus listas interiores 
// Libera la memoria asociada al vector de listas
void vaciar_hash_anterior(hash_t * hash){
	int i = 0;
	while(i < (hash->tamanio)){
		if(hash->tabla[i] != NULL){
			lista_destruir(hash->tabla[i], NULL);
		}
		i++;
	}
	free(hash->tabla);
}

// Recibe un hash inicializado
// Devuelve verdadero si se pudo redimensionarlo
// No se pudo redimensionar si no existe memoria suficiente para 
// un nuevo arreglo, para una nueva lista o si no se pudo insertar en una lista.
bool redimensionar_hash(hash_t * hash, size_t nuevo_tam){
	lista_t** nueva_lista = calloc(nuevo_tam, sizeof(lista_t*));// REVISAR
	if(!nueva_lista){
		return false;
	}
	size_t posicion = 0;
	bool se_pudo = true;
	nodo_hash_t *nodo;
	for (int i = 0; i < hash->tamanio; i++){
		if (hash->tabla[i]){
			lista_iter_t * iter = lista_iter_crear(hash->tabla[i]);
			nodo = lista_iter_ver_actual(iter);
			while(!lista_iter_al_final(iter) && se_pudo){
				nodo = lista_iter_ver_actual(iter);
				posicion = funcion_hash(nodo->clave) % nuevo_tam;
				if(nueva_lista[posicion] == NULL){
					nueva_lista[posicion] = lista_crear();
					if(nueva_lista[posicion] == NULL){
						free(nueva_lista);
						return false;
					}	
				}
				se_pudo = lista_insertar_ultimo(nueva_lista[posicion], nodo);
				if (!se_pudo){
					free(nueva_lista);
					return false;
				}
				lista_iter_avanzar(iter);
			}
			lista_iter_destruir(iter);
		}
	}
	vaciar_hash_anterior(hash);
	hash->tamanio = nuevo_tam;
	hash->tabla = nueva_lista;
	return true;
}

//
//Recibe el tamaño y la cantidad de elementos previamente inicializadas de un hash
// Devuelve el proximo numero primo que corresponda para que el factor de carga del hash
// Vuelva a su estado correcto
//
size_t prox_primo(hash_t * hash){
	size_t vector_redimension[18] = { 17, 31, 67, 127, 257, 509, 997, 2053, 4093, 8191, 16381, 32771, 65537, 131077, 262147, 524287, 1048589, 2097169};
	size_t proximo = vector_redimension[(hash->factor_redimension) + 1];
	hash->factor_redimension++;
	return proximo;
}
//Recibe el tamaño y la cantidad de elementos previamente inicializadas de un hash
// Devuelve el ANTERIOR numero primo que corresponda para que el factor de carga del hash
// Vuelva a su estado correcto
//
size_t ant_primo(hash_t *hash){
	size_t vector_redimension[18] = { 17, 31, 67, 127, 257, 509, 997, 2053, 4093, 8191, 16381, 32771, 65537, 131077, 262147, 524287, 1048589, 2097169};
	size_t anterior = vector_redimension[(hash->factor_redimension) - 1];
	hash->factor_redimension--;
	return anterior;
}
//Pre: todos los elementos que recibe deben estar incializados
// En el caso de la lista, se sabe que esta contiene un elemento que posee
// La misma clave que quiero insertar
// Devuelve verdadero si se pudo insertar.
bool reemplazar_dato(lista_t *lista, nodo_hash_t * nodo_insertar, hash_destruir_dato_t destructor){
	lista_iter_t * iter = lista_iter_crear(lista);
	if (!iter){
		return false;
	}
	bool continuar = true;
	nodo_hash_t *nodo_anterior = lista_iter_ver_actual(iter);
	while(!lista_iter_al_final(iter) && continuar){
		nodo_anterior = lista_iter_ver_actual(iter);
		if (strcmp(nodo_anterior->clave, nodo_insertar->clave) == 0){
			nodo_anterior = lista_iter_borrar(iter);
			lista_iter_insertar(iter, nodo_insertar);
			continuar = false;
		}
		lista_iter_avanzar(iter);
	}
	if (continuar == false){
		free(nodo_anterior->clave);
	}
	if (destructor != NULL){
		destructor(nodo_anterior->dato);
	}
	free(nodo_anterior);
	lista_iter_destruir(iter);
	return true;
}

/* Guarda un elemento en el hash, si la clave ya se encuentra en la
 * estructura, la reemplaza. De no poder guardarlo devuelve false.
 * Pre: La estructura hash fue inicializada
 * Post: Se almacenó el par (clave, dato)
 */
bool hash_guardar(hash_t *hash, const char *clave, void *dato){
	char *copia = strdup(clave);
	if (!copia){
		return false;
	}
	if ((hash->cantidad_elementos/hash->tamanio) >= FACTOR_DE_CARGA){
		size_t prox_tam = prox_primo(hash);
		redimensionar_hash(hash, prox_tam);
	}
	if ((hash->cantidad_elementos/hash->tamanio) <= FACTOR_DE_REDUCCION && hash->tamanio > TAMANIO_INICIAL ){
		size_t prox_tam = ant_primo(hash);
		redimensionar_hash(hash, prox_tam);
	}
	bool existe = hash_pertenece(hash, clave);
	size_t nueva_posicion = funcion_hash(copia) % hash->tamanio;
	nodo_hash_t * nodo = malloc(sizeof(nodo_hash_t));
	if (!nodo){
		free(copia);
		return false;
	}
	nodo->dato = dato;
	nodo->clave = copia;
	if (existe){
		existe = existe && reemplazar_dato(hash->tabla[nueva_posicion], nodo, hash->destructor);
		if (!existe){
			free(nodo);
			free(copia);
		}
		return existe;
	}
	if (hash->tabla[nueva_posicion] == NULL){
		lista_t * nueva_lista = lista_crear();
		if (!nueva_lista){
			free(nodo);
			free(copia);
			return false;
		}
		hash->tabla[nueva_posicion] = nueva_lista;
	}
	bool se_pudo = true;
	se_pudo = se_pudo && lista_insertar_ultimo(hash->tabla[nueva_posicion], nodo);
	if (!se_pudo){
		free(nodo);
		free(copia);
		return false;
	}
	hash->cantidad_elementos++;
	return se_pudo;
}


void * quitar_elemento(lista_t * lista, const char *clave){
	nodo_hash_t * nodo_quitado;
	lista_iter_t * iter = lista_iter_crear(lista);
	if (!iter){
		return NULL;
	}
	bool seguir = true;
	nodo_quitado = lista_iter_ver_actual(iter);
	while(!lista_iter_al_final(iter) && seguir){
		nodo_quitado = lista_iter_ver_actual(iter);
		if (strcmp(nodo_quitado->clave, clave) == 0){
			nodo_quitado = lista_iter_borrar(iter);
			seguir = false;
		}
		lista_iter_avanzar(iter);
	}
	lista_iter_destruir(iter);
	return nodo_quitado;
}


/* Borra un elemento del hash y devuelve el dato asociado.  Devuelve
 * NULL si el dato no estaba.
 * Pre: La estructura hash fue inicializada
 * Post: El elemento fue borrado de la estructura y se lo devolvió,
 * en el caso de que estuviera guardado.
 */
void *hash_borrar(hash_t *hash, const char *clave){
	bool pertenece = hash_pertenece(hash, clave);
	if (!pertenece){
		return NULL;
	}
	size_t posicion = funcion_hash(clave) % hash->tamanio;
	nodo_hash_t * nodo = quitar_elemento(hash->tabla[posicion], clave);
	if (!nodo){
		return NULL;
	}
	free(nodo->clave);
	void *dato = nodo->dato;
	free(nodo);
	hash->cantidad_elementos--;
	return dato;
}

//Recibe una clave y un nodo
//Devuelve verdadero si la clave es identica a la del nodo
bool es_clave_correcta(const char* clave, char* clavenodo){
	return (strcmp(clave, clavenodo) == 0);
}


/* Obtiene el valor de un elemento del hash, si la clave no se encuentra
 * devuelve NULL.
 * Pre: La estructura hash fue inicializada
 */
void *hash_obtener(const hash_t *hash, const char *clave){
	size_t posicion = funcion_hash(clave) % hash->tamanio;
	if (hash->tabla[posicion] == NULL){
		return NULL;
	}
	lista_iter_t * iter = lista_iter_crear(hash->tabla[posicion]);
	if (!iter){
		return NULL;
	}
	nodo_hash_t *nodo_copia = lista_iter_ver_actual(iter);;
	bool seguir = true;
	while(!lista_iter_al_final(iter) && seguir){
		nodo_copia = lista_iter_ver_actual(iter);
		seguir = !es_clave_correcta(clave, nodo_copia->clave);
		lista_iter_avanzar(iter);
	}
	if (lista_iter_al_final(iter) && seguir){
		lista_iter_destruir(iter);
		return NULL;
	}
	lista_iter_destruir(iter);
	return nodo_copia->dato;
}


/* Determina si clave pertenece o no al hash.
 * Pre: La estructura hash fue inicializada
 */
bool hash_pertenece(const hash_t *hash, const char *clave){
	size_t posicion_elemento;
	posicion_elemento=funcion_hash(clave) % hash->tamanio;

	if(hash->tabla[posicion_elemento] == NULL){
		return false;
	}
	lista_iter_t * iter = lista_iter_crear(hash->tabla[posicion_elemento]);
	if (!iter){
		return false;
	}
	nodo_hash_t * nodo_copia = lista_iter_ver_actual(iter);;
	bool seguir = true;
	while(!lista_iter_al_final(iter) && seguir){
		nodo_copia = lista_iter_ver_actual(iter);
		seguir = seguir && !es_clave_correcta(clave, nodo_copia->clave);
		lista_iter_avanzar(iter);
	}
	if (lista_iter_al_final(iter) && seguir){
		lista_iter_destruir(iter);
		return false;
	}
	lista_iter_destruir(iter);
	return true;

}

/* Devuelve la cantidad de elementos del hash.
 * Pre: La estructura hash fue inicializada
 */
size_t hash_cantidad(const hash_t *hash){
	return(hash->cantidad_elementos);
}

/* Destruye la estructura liberando la memoria pedida y llamando a la función
 * destruir para cada par (clave, dato).
 * Pre: La estructura hash fue inicializada
 * Post: La estructura hash fue destruida
 */
void hash_destruir(hash_t *hash){
	for (int i = 0; i < (hash->tamanio); i++){
		if (hash->tabla[i] != NULL){
			lista_iter_t * iter = lista_iter_crear(hash->tabla[i]);
			nodo_hash_t * nodo;
			while(!lista_iter_al_final(iter)){
				nodo = lista_iter_borrar(iter);
				if (hash->destructor != NULL){
					hash->destructor(nodo->dato);
				}
				free(nodo->clave);
				free(nodo);
				hash->cantidad_elementos--;
			}
			lista_iter_destruir(iter);
			lista_destruir(hash->tabla[i], free);
		}
	}
	free(hash->tabla);
	free(hash);
}





/* Iterador del hash */


// Crea iterador
hash_iter_t *hash_iter_crear(const hash_t *hash){
	size_t posicion_lista = 0;
	hash_iter_t *hash_iter = malloc(sizeof(lista_iter_t));
    if (hash_iter == NULL) {
        return NULL;
    }
    hash_iter->hash = hash;
	hash_iter->lista_iter = NULL;
	
	if (hash->cantidad_elementos == 0) {
		hash_iter->posicion=0;
		hash_iter->lista_iter=NULL;
		return hash_iter;
	}else{
		for (size_t i = 0; i < hash->tamanio-1; i++) {	
			if (hash->tabla[i]!= NULL) {
				if (!lista_esta_vacia(hash->tabla[i])){     ///////
					posicion_lista = i;
					break; 
				}
			}

		}

		hash_iter->lista_iter = lista_iter_crear(hash->tabla[posicion_lista]);
		}

	if (!hash_iter->lista_iter) {
		free(hash_iter);
		return NULL;
	}

	hash_iter->posicion=posicion_lista;
	return hash_iter;
		
}




// Devuelve clave actual, esa clave no se puede modificar ni liberar.
const char *hash_iter_ver_actual(const hash_iter_t *iter){
	if(!iter->lista_iter){
		return NULL;
	}
	if(lista_iter_al_final(iter->lista_iter)){
		 return NULL;
	}	 

	nodo_hash_t* nodo_actual = lista_iter_ver_actual(iter->lista_iter);
	return nodo_actual->clave;

}

// Comprueba si terminó la iteración
bool hash_iter_al_final(const hash_iter_t *iter){
	if (!iter->lista_iter) {
		return true;
	}
	return ((iter->posicion == (iter->hash->tamanio-1)) && (lista_iter_al_final(iter->lista_iter)));

}


// Avanza iterador
bool hash_iter_avanzar(hash_iter_t *iter){
	if(hash_iter_al_final(iter)){
		return false;
	}
	
	if(iter->lista_iter!=NULL){
		lista_iter_avanzar(iter->lista_iter);
		if (lista_iter_al_final(iter->lista_iter)){

			lista_iter_destruir(iter->lista_iter);
			iter->posicion++;
			int posicion_lista = -1;
			for (int i = (int)iter->posicion; i < iter->hash->tamanio; i++) {
				if (iter->hash->tabla[i]!= NULL) {			
					if (!lista_esta_vacia(iter->hash->tabla[i])){     ///////
						posicion_lista = i;
						break;
					}
				}
			}
			if(posicion_lista==-1){
				iter->lista_iter = NULL;
				return false;
			}

			iter->posicion = (size_t)posicion_lista;

			iter->lista_iter = lista_iter_crear(iter->hash->tabla[posicion_lista]);
			
			if (!iter->lista_iter){
				return false;
			}
		
		}
	}

   	return true;

}



// Destruye iterador
void hash_iter_destruir(hash_iter_t *iter){

	lista_iter_destruir(iter->lista_iter);
  	free(iter);


}



