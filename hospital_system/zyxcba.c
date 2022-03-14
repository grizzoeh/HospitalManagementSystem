#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "funciones_tp2.h"
#include "strutil.h"
#include "mensajes.h"
#include "csv.h"


#define COMANDO_PEDIR_TURNO "PEDIR_TURNO"
#define COMANDO_VACIO ""
#define COMANDO_ATENDER "ATENDER_SIGUIENTE"
#define COMANDO_INFORME "INFORME"
#define COMANDO_URGENCIA "URGENTE"
#define COMANDO_SIN_URGENCIA "REGULAR"

size_t cant_parametros(char** parametros){
   
    size_t cant = 0;
    while(parametros[cant] != NULL){
        cant++;
    }
    return cant;
}



void procesar_comando(const char* comando, char** parametros, clinica_t * clinica ) {
		hash_t *hash_pac = obtener_pacientes(clinica);
		hash_t *hash_espec = obtener_especialidades(clinica);
		abb_t * abb_doc = obtener_doctores(clinica);
	
	if (strcmp(comando, COMANDO_PEDIR_TURNO) == 0) {
		bool encolar = true;
		if(cant_parametros(parametros)!=3){
			printf(ENOENT_PARAMS, comando);
			return;
		}
		if (!hash_pertenece(hash_pac, parametros[0])){
			printf( ENOENT_PACIENTE, parametros[0]);
			encolar = false;
		}
		if (!hash_pertenece(hash_espec, parametros[1])){
			printf( ENOENT_ESPECIALIDAD, parametros[1]);
			encolar = false;
		}
		if(strcmp(parametros[2], COMANDO_URGENCIA) != 0 && strcmp(parametros[2], COMANDO_SIN_URGENCIA) != 0){
			printf( ENOENT_URGENCIA, parametros[2]);
			encolar = false;
			
		}
		else{
			if (encolar == true){	
				paciente_t * paciente = hash_obtener(hash_pac, parametros[0]);
				printf(PACIENTE_ENCOLADO, parametros[0]);
				if (strcmp(parametros[2], COMANDO_URGENCIA ) == 0){
					pedir_turno(clinica, paciente, parametros[1], true);
				}
				else{
					pedir_turno(clinica, paciente, parametros[1], false);
				}
			}
		}

	} else if (strcmp(comando, COMANDO_ATENDER) == 0) {
		if(cant_parametros(parametros)!=1){
			printf(ENOENT_PARAMS, comando);
			return;
		}
		if (!abb_pertenece(abb_doc, parametros[0])){
			printf(ENOENT_DOCTOR, parametros[0]);
		}
		else{
			atender(clinica, parametros[0]);
		}
	} else if (strcmp(comando, COMANDO_INFORME) == 0) {
		if(cant_parametros(parametros)<2){
			printf(ENOENT_PARAMS, comando);
			return;
		}
		if ((strcmp(COMANDO_VACIO,parametros[0]) == 0) && (strcmp(COMANDO_VACIO, parametros[1]) == 0)){
			crear_informe(clinica, true, NULL);
		}
		else{
			crear_informe(clinica, false, parametros);
		}

	} else {
		printf(ENOENT_CMD, comando);
	}
}

void eliminar_fin_linea(char* linea) {
	size_t len = strlen(linea);
	if (linea[len - 1] == '\n') {
		linea[len - 1] = '\0';
	}
}

void procesar_entrada(clinica_t* clinica){
	char* linea = NULL;
	size_t c = 0;
	while (getline(&linea, &c, stdin) > 0) {
		eliminar_fin_linea(linea);
		char** campos = split(linea, ':');
		if (campos[1] == NULL) {
			printf(ENOENT_FORMATO, linea);
			free_strv(campos);
			continue;	
		}
		char** parametros = split(campos[1], ',');
		procesar_comando(campos[0], parametros, clinica);
		free_strv(parametros);
		free_strv(campos);
	}
	free(linea);
}


int main(int argc, char** argv) {
	if (argc != 3){
		printf(ENOENT_CANT_PARAMS);
		return 1;

	}

	void * extra = NULL;
	//char * recib = "doctores.csv";
	lista_t *lista_doctores = csv_crear_estructura(argv[1], creador_doctor, extra);
	if (!lista_doctores){
		printf(ENOENT_ARCHIVO, argv[1]);
	
		return 1;
	}
	//recib = "pacientes.csv";
	lista_t *lista_pacientes = csv_crear_estructura(argv[2], creador_paciente, extra);
	if (!lista_pacientes){
		printf(ENOENT_ARCHIVO, argv[2]);
		lista_destruir(lista_doctores, NULL);
		return 1;
	
	}	
	clinica_t *clinica = clinica_crear();
	if(!clinica){
		return 1;
	}
	hash_t *hash_pac = obtener_pacientes(clinica);
	hash_t *hash_espec = obtener_especialidades(clinica);
	abb_t * abb_doc = obtener_doctores(clinica);

	if(!parsear_pacientes(hash_pac,lista_pacientes)){
		lista_destruir(lista_pacientes,NULL);
		lista_destruir(lista_doctores,NULL);
		clinica_destruir(clinica);
		return 1;
	}

	if(!parsear_doctores(abb_doc,lista_doctores)){
		lista_destruir(lista_pacientes,NULL);
		lista_destruir(lista_doctores,NULL);
		clinica_destruir(clinica);
		return 1;
	}
	if(!parsear_especialidades(hash_espec,lista_doctores)){
		lista_destruir(lista_pacientes,NULL);
		lista_destruir(lista_doctores,NULL);
		clinica_destruir(clinica);
		return 1;
	}
	lista_destruir(lista_doctores, NULL); 
	lista_destruir(lista_pacientes, NULL);
	procesar_entrada(clinica);
	
	clinica_destruir(clinica);

//valgrind  --leak-check=full --track-origins=yes --show-reachable=yes ./pruebas


	return 0;
}
