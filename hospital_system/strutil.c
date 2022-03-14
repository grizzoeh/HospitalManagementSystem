#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include "strutil.h"


char * substr(const char *str, size_t n){
	char * nueva_cadena = strndup(str, n);
	return nueva_cadena;
}
 


char **split(const char *str, char sep){
	if (str == NULL){
		return NULL;
	}
	size_t largo_max = strlen(str);
	size_t vector_posiciones[largo_max + 1];
	for (int i = 0; i < largo_max + 2; i++){
		vector_posiciones[i] = 0;
	}
	vector_posiciones[0] = (size_t) 1;
	size_t tope = 1;
	size_t contador_largo = 0;
	size_t contador_separador = 2; // Se inicializa este valor en 2, ya que aunque no haya nunca 
	char leido = str[contador_largo]; //un caracter de separacion, igualmente habra que dividir la candena
	while(leido != '\0'){
		if (leido == sep){
			contador_separador++;
			vector_posiciones[tope] = contador_largo;
			tope ++;
		}
		contador_largo++;
		leido = str[contador_largo];
	}
	char ** nueva_cadena = malloc(sizeof(char*) * (contador_separador ));
	if (!nueva_cadena){
		return NULL;
	}
	bool no_fallos = true;
	if (contador_separador == 2){
		size_t largo_actual = strlen(str);
		nueva_cadena[0] = substr(str, (largo_actual+1));
	}
	else{
		size_t separaciones = 0;
		nueva_cadena[separaciones] = substr(str, vector_posiciones[1]);
		if (nueva_cadena[separaciones] == NULL ){
			no_fallos = false;
		}
		separaciones++;
		while (separaciones < contador_separador - 1 && no_fallos){
			nueva_cadena[separaciones] = substr(str + vector_posiciones[separaciones] + 1, (vector_posiciones[separaciones + 1] - vector_posiciones[separaciones] - 1));
			if (nueva_cadena[separaciones] == NULL ){
				no_fallos = false;
			}
			separaciones++;
		}
	}
	if (no_fallos == false){
		return NULL;
	}
	nueva_cadena[contador_separador - 1]  = NULL;
	return nueva_cadena;

}

char *join(char **strv, char sep){
	if (!strv){
		return NULL;
	}
	size_t contador = 0;
	size_t largo = 0;
	while (strv[contador] != NULL){
		largo = largo + strlen(strv[contador]);
		contador++;
	}
	char * fusion_cadena = calloc(largo * sizeof(char) + contador + 1, sizeof(char));
	if (!fusion_cadena){
		return NULL;
	}
	size_t nuevo_contador = 0;
	size_t tope = 0;
	while (strv[nuevo_contador] != NULL){
		strcpy(fusion_cadena + tope, strv[nuevo_contador]);
		tope = (tope + strlen(strv[nuevo_contador]));
		if (sep != '\0'){
			fusion_cadena[tope] = sep;
			tope++;
		}
		nuevo_contador++;
	}
	if (sep != '\0' && contador != 0){
		fusion_cadena[tope - 1] = '\0';
	}
	return fusion_cadena;
}



void free_strv(char * strv[]){
	if (!strv){
		return;
	}
	size_t contador = 0;
	while(strv[contador] != NULL){
		free(strv[contador]);
		contador ++;
	}
	free(strv);
}
