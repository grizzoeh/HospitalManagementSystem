#ifndef __ABB_H__
#define __ABB_H__

#include <stdbool.h>
#include <stddef.h>
#include "pila.h"





typedef struct abb abb_t;

typedef int (*abb_comparar_clave_t) (const char *, const char *);
typedef void (*abb_destruir_dato_t) (void *);



/*Crea un arbol asignandole su memoria, si falla devuelve NULL
 * Debe recibir dos parametros, un comparador de clave y un destructor de dato
 */
abb_t* abb_crear(abb_comparar_clave_t cmp, abb_destruir_dato_t destruir_dato);


/* Guarda un elemento en el arbol, si la clave ya se encuentra en la
 * estructura, la reemplaza. De no poder guardarlo devuelve false.
 * Pre: La estructura fue inicializada
 * Post: Se almacenó el par (clave, dato)
 */
bool abb_guardar(abb_t *arbol, const char *clave, void *dato);


/* Borra un elemento del arbol y devuelve el dato asociado.  Devuelve
 * NULL si el dato no estaba.
 * Pre: La estructura fue inicializada
 * Post: El elemento fue borrado de la estructura y se lo devolvió,
 * en el caso de que estuviera guardado.
 */
void *abb_borrar(abb_t *arbol, const char *clave);


/* Obtiene el valor de un elemento del arbol, si la clave no se encuentra
 * devuelve NULL.
 * Pre: La estructura fue inicializada
 */
void *abb_obtener(const abb_t *arbol, const char *clave);


/*Devuelve verdadero si la clave pertenece al arbol
 *Pre: La estructura fue inicializada
 */
bool abb_pertenece(const abb_t *arbol, const char *clave);


/* Devuelve la cantidad de elementos del arbol.
 * Pre: La estructura fue inicializada
 */
size_t abb_cantidad( abb_t *arbol);


/* Destruye la estructura liberando la memoria pedida y llamando a la función
 * destruir para cada par (clave, dato).
 * Pre: La estructura arnol fue inicializada
 * Post: La estructura arbol fue destruida
 */
void abb_destruir(abb_t *arbol);


//==============================================================================


void abb_in_order(abb_t *arbol, bool visitar(const char *, void *, void *), void *extra);


//==============================================================================


typedef struct abb_iter abb_iter_t;

// Crea iterador
abb_iter_t *abb_iter_in_crear(const abb_t *arbol);

// Avanza iterador
bool abb_iter_in_avanzar(abb_iter_t *iter);

// Devuelve clave actual
const char *abb_iter_in_ver_actual(const abb_iter_t *iter);

// Comprueba si terminó la iteración
bool abb_iter_in_al_final(const abb_iter_t *iter);

// Destruye iterador
void abb_iter_in_destruir(abb_iter_t* iter);


#endif  // HASH_H
