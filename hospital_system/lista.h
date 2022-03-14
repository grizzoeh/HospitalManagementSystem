#ifndef LISTA_H
#define LISTA_H


#include <stdbool.h>
#include <stddef.h>

/* ******************************************************************
 *                DEFINICION DE LOS TIPOS DE DATOS
 * *****************************************************************/

/* La lista está planteada como una lista de punteros genéricos. */

struct lista;
typedef struct lista lista_t;

struct lista_iter;
typedef struct lista_iter lista_iter_t;


/* ******************************************************************
 *                    PRIMITIVAS DE LA LISTA
 * *****************************************************************/

// Crea una lista.
// Post: devuelve una nueva lista vacía.
lista_t *lista_crear(void);

// Destruye la lista. Si se recibe la función destruir_dato por parámetro,
// para cada uno de los elementos de la lista llama a destruir_dato.
// Pre: la lista fue creada. destruir_dato es una función capaz de destruir
// los datos de la lista, o NULL en caso de que no se la utilice.
// Post: se eliminaron todos los elementos de la lista.
void lista_destruir(lista_t *lista, void (*destruir_dato)(void *));


// Devuelve verdadero si la lista no tiene elementos, false en caso contrario.
// Pre: la lista fue creada.
bool lista_esta_vacia(const lista_t *lista);


// Obtiene el valor del primer elemento de la lista. Si la lista tiene
// elementos, se devuelve el valor del primero, si está vacía devuelve NULL.
// Pre: la lista fue creada.
// Post: se devolvió el primer elemento de la lista, cuando no está vacía.
void *lista_ver_primero(const lista_t *lista);

// Obtiene el valor del ultimo elemento de la lista. Si la lista tiene
// elementos, se devuelve el valor del primero, si está vacía devuelve NULL.
// Pre: la lista fue creada.
// Post: se devolvió el ultimo elemento de la lista, cuando no está vacía.
void *lista_ver_ultimo(const lista_t *lista);

// Obtiene el largo la lista. si está vacía devuelve 0.
// Pre: la lista fue creada.
size_t lista_largo(const lista_t *lista);


// Agrega un nuevo elemento al principio de la lista. Devuelve falso en caso de error.
// Pre: la lista fue creada.
// Post: se agregó un nuevo elemento a la lista, valor se encuentra al principio
// de la lista.
bool lista_insertar_primero(lista_t *lista, void *dato);

// Agrega un nuevo elemento al final de la lista. Devuelve falso en caso de error.
// Pre: la lista fue creada.
// Post: se agregó un nuevo elemento a la lista, valor se encuentra al final
// de la lista.
bool lista_insertar_ultimo(lista_t *lista, void *dato);



// Saca el primer elemento de la lista. Si la lista tiene elementos, se quita el
// primero de la lista, y se devuelve su valor, si está vacía, devuelve NULL.
// Pre: la lista fue creada.
// Post: se devolvió el valor del primer elemento anterior, la lista
// contiene un elemento menos, si la lista no estaba vacía.
void *lista_borrar_primero(lista_t *lista);




/* ******************************************************************
*                    PRIMITIVAS DEL ITERADOR EXTERNO
* *****************************************************************/

//mientras no devuelva false, itera hasta el último elemento de la lista,
//Pre: la lista fue creada.
void lista_iterar(lista_t* lista, bool visitar(void* dato, void* extra), void* extra);




/* ******************************************************************
 *                    PRIMITIVAS DEL ITERADOR EXTERNO
 * *****************************************************************/
// Crea un iterador al cual se le pasa por parametro una lista.
// Pre: la lista fue creada. 
// Post: devuelve un iterador que comienza con el primero de la lista.
lista_iter_t *lista_iter_crear(lista_t *lista);

//Avanza el iterador una posicion
// Pre: el iterador fue creado.
// Post: Devuele false en caso de que la posicion a la cual se itera sea NULL, en caso contrario
//devuelve true.
bool lista_iter_avanzar(lista_iter_t *iter);



//Devuelve el dato de la posicion donde esta situado el iterador (pre: el iterador fue creado) 
// y NULL si el iterador esta en una posicion vacia.
void *lista_iter_ver_actual(const lista_iter_t *iter);


//Devuelve true si el iterador recorrio toda la lista y se encuentra en la posicion del final (NULL)
//caso contrario devuelve false.
bool lista_iter_al_final(const lista_iter_t *iter);



// Destruye el iterador.
// Pre: el iterador fue creado.
// Post: se eliminó el iterador.
void lista_iter_destruir(lista_iter_t *iter);

//Inserta un nuevo elemento en la posicion del iterador.
//Pre: el iterador fue creado.
bool lista_iter_insertar(lista_iter_t *iter, void *dato);

//Borra el elemento ubicado en la posicion del iterador.
//Pre: el iterador fue creado.
void *lista_iter_borrar(lista_iter_t *iter);










/* *****************************************************************
 *                      PRUEBAS UNITARIAS
 * *****************************************************************/

// Realiza pruebas sobre la implementación realizada.
//
// Las pruebas deben emplazarse en el archivo ‘pruebas_lista.c’, y
// solamente pueden emplear la interfaz pública tal y como aparece en lista.h
// (esto es, las pruebas no pueden acceder a los miembros del struct lista).
//
// Para la implementación de las pruebas se debe emplear la función
// print_test(), como se ha visto en TPs anteriores.
void pruebas_lista_estudiante(void);

#endif  // LISTA_H
