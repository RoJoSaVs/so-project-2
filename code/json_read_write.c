#include <stdio.h>
#include "cJSON.h"
#include "cJSON.c"

cJSON *root;
FILE *archivo;
char nombre_archivo[100];

int jsonRead(){
    // Leer el archivo JSON
    

    archivo = fopen("files/stats.json", "r");

    if (archivo == NULL) {
        printf("No se pudo abrir el archivo.\n");
        return 1;
    }

    fseek(archivo, 0, SEEK_END);
    long archivo_tamano = ftell(archivo);
    rewind(archivo);

    char *json_str = (char*) malloc(sizeof(char) * archivo_tamano);
    fread(json_str, archivo_tamano, 1, archivo);

    fclose(archivo);

    // Parsear el archivo JSON
    root = cJSON_Parse(json_str);

    // Obtener el tamaño del arreglo
    //int tamano = cJSON_GetArraySize(root);

    // Crear un array de objetos JSON para almacenar los datos
    //cJSON **datos = (cJSON**) malloc(sizeof(cJSON*) * tamano);

    // Recorrer el arreglo y agregar los objetos al array
    //int i;
    //for (i = 0; i < tamano; i++) {
    //    cJSON *objeto = cJSON_GetArrayItem(root, i);
    //    datos[i] = objeto;
    //}

    // Imprimir los datos
    //printf("Los datos del arreglo son:\n");
    //for (i = 0; i < tamano; i++) {
    //   cJSON *objeto = datos[i];
    //    printf("Objeto %d:\n", i + 1);
    //    printf("  Campo1: %s\n", cJSON_GetObjectItem(objeto, "server")->valuestring);
    //    printf("  Campo2: %d\n", cJSON_GetObjectItem(objeto, "totalRequest")->valueint);
    //}
    //printf("\n");


    return 0; 
}

int jsonWrite(char const * name,int totalrequest, double timeExecution,int averageRequestTime, int memoryConsumption, int totalBytesSend){
     // Crear los objetos JSON
    cJSON *obj1 = cJSON_CreateObject();
    cJSON *arr = cJSON_CreateArray();

    // Agregar los datos a los objetos JSON
    cJSON_AddStringToObject(obj1, "server", name);
    cJSON_AddNumberToObject(obj1, "totalRequest", totalrequest);
    cJSON_AddNumberToObject(obj1, "timeExecution", timeExecution);
    cJSON_AddNumberToObject(obj1, "averageRequestTime", averageRequestTime);
    cJSON_AddNumberToObject(obj1, "memoryConsumption", memoryConsumption);
    cJSON_AddNumberToObject(obj1, "totalBytesSend", totalBytesSend);

    cJSON_AddItemToArray(root, obj1);

    // Escribir el archivo JSON

    archivo = fopen("files/stats.json", "w");

    if (archivo == NULL) {
        printf("No se pudo abrir el archivo.\n");
        return 1;
    }

    char *json_str = cJSON_Print(root);
    fprintf(archivo, "%s", json_str);
    fclose(archivo);
    return 0;
}

int save(char const * name, int totalrequest, double timeExecution, int averageRequestTime, int memoryConsumption, int totalBytesSend){
    
    int readRes = jsonRead();
    int writeRes = jsonWrite(name, totalrequest, timeExecution, averageRequestTime, memoryConsumption, totalBytesSend);
    return 0;
}

