#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <conio.h>
#include "list.h"

typedef struct{
    char nombre[31];
    char artista[31];
    char** genero;
    size_t cantGeneros;
    size_t anyo;
} Cancion;

typedef struct{
    char nombre[31];
    size_t cantCanciones;
    List* canciones;
} Lista;

Cancion* createCancion(){
    Cancion* cancion = ( Cancion* ) malloc( sizeof( Cancion ) );
    if(!cancion) {
        printf("\nError al crear tipo cancion\n"); 
        getch();
        exit(1);
    }
    return cancion;
}

Lista* crearLista( char* nombre ){

    Lista* lista = (Lista*) malloc(sizeof(Lista));
    strcpy(lista->nombre, nombre);
    lista->cantCanciones = 0;
    lista->canciones = createList();

    return lista;
}

const char *get_csv_field (char * tmp, int k) {
    int open_mark = 0;
    char* ret=(char*) malloc (100*sizeof(char));
    int ini_i=0, i=0;
    int j=0;
    while(tmp[i+1]!='\0'){

        if(tmp[i]== '\"'){
            open_mark = 1-open_mark;
            if(open_mark) ini_i = i+1;
            i++;
            continue;
        }

        if(open_mark || tmp[i]!= ','){
            if(k==j) ret[i-ini_i] = tmp[i];
            i++;
            continue;
        }

        if(tmp[i]== ','){
            if(k==j) {
               ret[i-ini_i] = 0;
               return ret;
            }
            j++; ini_i = i+1;
        }

        i++;
    }

    if(k==j) {
       ret[i-ini_i] = 0;
       return ret;
    }


    return NULL;
}

size_t contarGeneros( char* strGeneros ){
    size_t generos = 1;
    size_t indice = 0;
    while(strGeneros[indice] != '\0'){
        if (strGeneros[indice] == ','){
            generos += 1;
        }
        
        indice += 1;
    }
    return generos;
}

char* eliminarEspacios( char* str ){

    size_t largo = strlen(str);
    size_t j = 0;
    for(size_t i = 0; i < largo + 1; i++){
        if(str[i] != ' '){
            
            str[j] = str[i];
            j += 1;
        }
    }
    return str;
}

Lista* buscarLista(List* listas , size_t* cantListas , char* nombre){
    Lista* listaActual = NULL;
    for(size_t i = 0; i < *cantListas; i++){
        if(i == 0){
            listaActual = firstList(listas);
        }
        else{
            listaActual = nextList(listas);
        }
        if(strcmp(listaActual->nombre, nombre) == 0 ) return listaActual;
    }

    return NULL;
}

bool estaCancion(List* listaCanciones, size_t cantCanciones, char* nombre, char* artista){
    Cancion* cancionActual = NULL;
    for(size_t i = 0; i < cantCanciones; i++){
        if(i == 0){
            cancionActual = firstList(listaCanciones);
        }
        else{
            cancionActual = nextList(listaCanciones);
        }
        if(strcmp(cancionActual->nombre, nombre) == 0 && strcmp(cancionActual->artista, artista) == 0 ) return true;
    }

    return false;
}

void agregarCancion(List* listas, size_t* ptrCantListas, Cancion *cancionActual, char* nombreLista, bool desdeMenu){
    char reintento[2];
    if( *ptrCantListas == 0 ){
        if(desdeMenu){
            printf("\n\nNO HA IMPORTADO AUN");
            printf("\nDesea agregarla? [s/n]: ");
            scanf("%s", reintento);
            if(strcmp( reintento, "n" ) == 0){
                printf("\nCancion no agregada");
                return;
            }
        }
        Lista* nuevaLista = crearLista( nombreLista );
        pushBack(nuevaLista->canciones, cancionActual);
        nuevaLista->cantCanciones += 1;

        pushBack(listas , nuevaLista);
        *ptrCantListas += 1;
    }
    else{
        Lista* listaActual = (Lista *)buscarLista(listas , ptrCantListas , nombreLista);
        if( listaActual  == NULL){
            if(desdeMenu){
                
                printf("\n\nNO EXISTE UNA LISTA CON ESE NOMBRE");
                printf("\nDesea agregarla? [s/n]: ");
                scanf("%s", reintento);
                if(strcmp( reintento, "n" ) == 0){
                    printf("\nCancion no agregada");
                    return;
                }
            }
            listaActual = crearLista( nombreLista );
            pushBack(listaActual->canciones, cancionActual);
            listaActual->cantCanciones += 1;

            pushBack(listas , listaActual);
            *ptrCantListas += 1;
        }
        else{
            if( !estaCancion(listaActual->canciones, listaActual->cantCanciones, cancionActual->nombre, cancionActual->artista) ){
                pushBack(listaActual->canciones, cancionActual);
                listaActual->cantCanciones += 1;
            }
            else{
               if(desdeMenu){
                   printf("\n\nYA EXISTE UNA CANCION CON ESE NOMBRE EN LA LISTA\n");
                   printf("\nCancion no agregada");
                   return;
               } 
            }
        }
    }
    if(desdeMenu) printf("\nCancion agregada con exito");
    
}

void importar( List* listas, size_t* ptrCantListas ){ 
    FILE* fp = NULL;
    bool errorArchivo = true;
    while(errorArchivo){
        system("cls");
        printf("\n========== IMPORTANDO ARCHIVO CSV ==========\n");
        char nombreArchivo[31];
        printf("\nIngrese el nombre del archivo: ");
        scanf("%s",  nombreArchivo );
        fp = fopen ( strcat( nombreArchivo, ".csv" ), "r");

        if(!fp){
            char reintento[2];
            printf("\nERROR AL ABRIR EL ARCHIVO O NO EXISTE\n");
            printf("No es necesario que escriba la extension .csv\n");
            printf("Desea intentarlo nuevamente? [s/n]: ");
            scanf("%s", reintento);

            if(strcmp( reintento, "n" ) == 0){
                printf("\n========== VOLVIENDO AL MENU ==========\n");
                getch();
                return;
            } 
        }
        else{
            errorArchivo = false;
        }
    }
    char linea[1024];
    char nombreLista[31];
    char* aux;
    size_t cantGeneros;
    while (fgets (linea, 1023, fp) != NULL) { 

        Cancion* cancionActual = createCancion();
        
        strcpy( cancionActual->nombre, aux = (char *) get_csv_field(linea, 0) );
        strcpy( cancionActual->artista, aux = (char *) get_csv_field(linea, 1) );
        aux = (char *) get_csv_field(linea, 2);
        cantGeneros = contarGeneros( aux );
        cancionActual->cantGeneros = cantGeneros;
        cancionActual->genero = (char**) malloc(cantGeneros * sizeof(char*));

        for(size_t i = 0; i < cantGeneros; i++){
            cancionActual->genero[i] = (char*) malloc(31 * sizeof(char));
        }

        if (cantGeneros == 1){
            strcpy( cancionActual->genero[0], aux);
        }
        else{
            aux = strcat( aux, "\n" );
            for( size_t i = 0 ; i < cantGeneros ; i++){ 
                strcpy( cancionActual->genero[i], eliminarEspacios( (char*) get_csv_field(aux, i))  );
            }
        }
        cancionActual->anyo = atoi(aux = (char *) get_csv_field(linea, 3));
        strcpy( nombreLista , aux = (char *) get_csv_field(linea, 4) );

        agregarCancion(listas, ptrCantListas, cancionActual, nombreLista, false);
        free(aux);
        
    }
    fclose(fp);
    printf("\n========== IMPORTADO CON EXITO ==========\n");
    getch();
}

void exportar( List* listas, size_t* ptrCantListas ){
    
    if( *ptrCantListas == 0 ){
        system("cls");
        printf("\n========== EXPORTANDO ARCHIVO CSV ==========\n");
        printf("\n   NO EXISTE NINGUNA LISTA DE REPRODUCCION\n");
        printf("\n============= VOLVIENDO AL MENU ============\n");
        getch();
        return;
    }

    FILE* fp = NULL;
    bool errorArchivo = true;
    while(errorArchivo){
        system("cls");
        printf("\n========== EXPORTANDO ARCHIVO CSV ==========\n");
        char nombreArchivo[31];
        printf("\nIngrese el nombre del archivo: ");
        scanf("%s",  nombreArchivo );
        fp = fopen ( strcat( nombreArchivo, ".csv" ), "w");

        if(!fp){
            char reintento[2];
            printf("\nERROR AL ABRIR EL ARCHIVO O NO EXISTE\n");
            printf("No es necesario que escriba la extension .csv\n");
            printf("Desea intentarlo nuevamente? [s/n]: ");
            scanf("%s", reintento);

            if(strcmp( reintento, "n" ) == 0){
                printf("\n========== VOLVIENDO AL MENU ==========\n");
                getch();
                return;
            } 
        }
        else{
            errorArchivo = false;
        }
    }
    
    Lista* listaActual;
    Cancion* cancionActual;
    for(size_t i = 0; i < *ptrCantListas; i++){
        if(i == 0){
            listaActual = firstList(listas);
        }
        else{
            listaActual = nextList(listas);
        }
        
        for(size_t j = 0; j < listaActual->cantCanciones; j++){
        
            if(j == 0){
                cancionActual = firstList(listaActual->canciones);
            }
            else{
                cancionActual = nextList(listaActual->canciones);
            }
            
            fprintf(fp ,"%s,", cancionActual->nombre);
            fprintf(fp ,"%s,", cancionActual->artista);
            if(cancionActual->cantGeneros == 1){
                fprintf(fp ,"%s,", cancionActual->genero[0]);
            }
            else{
                fprintf(fp ,"\"");
                for(size_t k = 0; k < cancionActual->cantGeneros; k++){
                    if(k == 0) fprintf(fp ,"%s,", cancionActual->genero[k]);
                    else if(k == cancionActual->cantGeneros-1) fprintf(fp ," %s", cancionActual->genero[k]);
                    else  fprintf(fp ," %s,", cancionActual->genero[k]);
                }
                fprintf(fp ,"\",");
            }
            fprintf(fp ,"%i,", cancionActual->anyo);
            fprintf(fp ,"%s", listaActual->nombre);
            fprintf(fp ,"\n");
        }
    }
    fclose(fp);
    printf("\n========== EXPORTADO CON EXITO ==========\n");
    getch();

}

void menuAgregarCancion(List* listas, size_t* ptrCantListas){
    char nombreLista[31];
    char aux[31];
    bool desdeMenu = true;
    
    while(true){
        Cancion* nuevaCancion = createCancion();
        char reintento[2];
        system("cls");
        printf("\n==========  AGREGAR CANCION ==========\n");
        printf("\nDEBE INGRESAR LOS DATOS DE LA CANCION\n");
        printf("\nNombre de la cancion: ");
        scanf("%s", aux);
        strcpy(nuevaCancion->nombre, aux);
        printf("\nArtista de la cancion: ");
        scanf("%s", nuevaCancion->artista);
        printf("\nCuantos generos tiene la cancion: ");
        scanf("%i", &nuevaCancion->cantGeneros);
        
        
        nuevaCancion->genero = (char**) malloc( nuevaCancion->cantGeneros * sizeof(char*) );
        for(size_t i = 0; i < nuevaCancion->cantGeneros; i++){
            nuevaCancion->genero[i] = (char*) malloc(31 * sizeof(char));
            printf("\nIngrese el genero %i: ", i+1); 
            scanf("%s", nuevaCancion->genero[i] );
        }
        
        printf("\nAño de la cancion: ");
        scanf("%i", &nuevaCancion->anyo);
        printf("\nLista en la que desea agregarla: ");
        scanf("%s", nombreLista);
        agregarCancion(listas, ptrCantListas, nuevaCancion, nombreLista, desdeMenu);
        printf("\nProceso terminado");
        printf("\nDesea intentarlo nuevamente? [s/n]: ");
        scanf("%s", reintento);
        if(strcmp( reintento, "n" ) == 0){
            break;
        }
    }
    
    printf("\n========== VOLVIENDO AL MENU ==========\n");
    getch();
    
}

int main() { 

    List *Listas_de_reproduccion = createList();
    size_t cantListas = 0;

    int opcion = -1;
    while(opcion != 11) {
        system("cls");
        printf("\n========== MENU DE CANCIONES ==========\n");
        printf("\n1.-  Importar canciones\n");
        printf("2.-  Exportar canciones\n");
        printf("3.-  Agregar cancion\n");
        printf("4.-  Buscar cancion por nombre\n");  
        printf("5.-  Buscar cancion por artista\n"); 
        printf("6.-  Buscar cancion por genero\n");  
        printf("7.-  Eliminar cancion\n");
        printf("8.-  Mostrar listas de reproduccion\n");
        printf("9.-  Mostrar una lista en concreto\n");
        printf("10.- Mostrar todas las canciones\n");
        printf("11.- Salir\n");

        printf("\nINGRESE SU OPCION: ");
        scanf("%i", &opcion);

        switch(opcion)
        {
            case 1: 
                importar(Listas_de_reproduccion, &cantListas);
                break;
            case 2: 
                exportar(Listas_de_reproduccion, &cantListas);
                break;
            case 3: 
                menuAgregarCancion(Listas_de_reproduccion, &cantListas);
                break;/*
            case 5: 
                buscarNombre(Listas_de_reproduccion, cantListas);
                break;
            case 6: 
                buscarArtista(Listas_de_reproduccion, cantListas);
                break;
            case 7: 
                buscarGeneros(Listas_de_reproduccion, cantListas);
                break;
            case 8: 
                eliminarCancion(Listas_de_reproduccion, cantListas);
                break;
            case 9: 
                mostrarListas(Listas_de_reproduccion, cantListas);
                break;
            case 10: 
                mostrarRepertorio(Listas_de_reproduccion, cantListas);
                break;*/
            case 11:
                return 0;
        }
    }
    

    return EXIT_SUCCESS;
}

/*
RECUERDA PONER OPCION DE VOLVER EN CADA FUNCION, EXCEPTO IMPORTAR Y EXPORTAR

void importar(musica)
abrir archivo csv, guardar en un tipo cancion la que esta en primera
posicion, de esta tipo cancion leer el nombre de la lista en la que
se ubica la canción; si el repertorio no contiene elementos, entonces
crear una lista con ese nombre, ponerla en el repertorio, y poner 
como primer elemento esa canción; si el repertorio contiene elementos,
buscar una lista del mismo nombre; si no existe, entonces crear una 
lista con ese nombre, ponerla en el repertorio, y poner como primer 
elemento esa canción; si existe (opcional), *entonces comparar el nombre 
de la canción con alguno de los que se encuentran en la lista para 
saltar una alerta de cancion repetida y no ingresarla; si no esta repetida* 
ingresar la cancion a la lista 
(RECUERDA AUMENTAR 1 EN CANTIDAD, EN CASO DE QUE SE INGRESE UN NUEVO 
ELEMENTO A UNA LISTA)
(REVISAR EL TEMA DE GENEROS) 

void exportar(musica)
abrir archivo csv en modo escritura, abrir el repertorio de musica, abrir 
en orden las listas y escribir en orden las componentes de las canciones.

void agregarCancion(musica)
recibira datos para un tipo cancion, comparara la componente de nombre
de lista con las listas existentes en el repertorio; si no existe, 
entonces agregar una nueva lista con ese nombre y la cancion;
si existe, entonces comparar por nombre de cancion; si existe, mostrar una
alerta y no agregarla; si no existe, agregarla a la lista. Preguntar
si se desea agregar nuevamente una cancion.

void buscarCancion(musica)
preguntar si se desea buscar por nombre, artista o genero y solo buscar
en cada lista que se encuentre en el repertorio y mostrar en pantalla
las que se encuentren (si en las listas de genero se encuentra el genero
entre otros más, igual se muestra)

void eliminarCancion(musica)
certeramente buscará entre las listas para encontrar alguna cancion que
tenga mismo nombre, artista y año que la que se desea eliminar, ubicar el
current en esta y relizar un popcurrent, disminuir la cantidad de esa lista

void mostrarNombredeListas(musica)
pasará por el repertorio escribiendo el nombre de las listas en pantalla

void mostrarlista(musica)
recibira un nombre de lista; si no existe se pedirá denuevo; si existe,
mostrar toda la musica de tal lista en pantalla

void mostrarRepertorio(musica)
parecido al exportar pero en lugar de escribir, mostrará en pantalla

*/