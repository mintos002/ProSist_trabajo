/**
* Para compilar teclea: gcc x.c -lpthread -o x
*/
#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#include <time.h>
#include "buffer_circ.h"

#define TG1 1         // Definir tiempos
#define TG2 2
#define TP1 2
#define TP2 2
#define TP3 2
#define TP4 2
#define TP5 2
#define TSTATUS 2
#define NUMPIEZAS 11  // Definir n piezas
#define TDEFBAR 2     // Tiempo (seg) barnizado defectuoso
#define TDEFSEC 4     // Tiempo (seg) secado defectuoso
#define TDEFCOC 6     // Tiempo (seg) cocido defectuoso


struct Buffer_Circ almacen1, almacen2, almacen3, almacen4, almacen5; // Definir Buffer_Circ

int nGenerados = 0;
int nSacados = 0;
int nDescartados = 0;

sem_t mutex, items, huecos;           // Definir semaforos

long getCurrentMicroseconds(){        // Obtener tiempo
	struct timespec currentTime;
	clock_gettime(CLOCK_MONOTONIC, &currentTime);
	return (currentTime.tv_sec)*1000000 + (currentTime.tv_nsec) / 1000;
}

void *printStatus(void *args){        // Mostrar el estado de los almacenes cada TSTATUS
  while(1){
    printf("---------------------------------------------------------------\n");
    printf("                     ESTADO DE ALMACENES                       \n");
    printf("---------------------------------------------------------------\n");
    printf("Almacen 1: ");
    print(&almacen1);
    printf("---   ---   ---   ---   ---   ---   ---   ---   ---   ---   ---\n");
    printf("Almacen 2: ");
    print(&almacen2);
    printf("---   ---   ---   ---   ---   ---   ---   ---   ---   ---   ---\n");
    printf("Almacen 3: ");
    print(&almacen3);
    printf("---   ---   ---   ---   ---   ---   ---   ---   ---   ---   ---\n");
    printf("Almacen 4: ");
    print(&almacen4);
    printf("---   ---   ---   ---   ---   ---   ---   ---   ---   ---   ---\n");
    printf("Almacen 5: ");
    print(&almacen5);
    printf("---------------------------------------------------------------\n");
    
    sleep(TSTATUS);
  }
}

void *generaPiezas1(void *args){                    // Generar piezas cada TG1 ms
  pieza pieza;
  int err;
  while(nGenerados < NUMPIEZAS){                    // Mientras queden piezas por generar
    pieza.linea = 1;                                  // La pieza es de la linea 1
    pieza.tNacido = getCurrentMicroseconds();       // Iniciar t entrada
    pieza.tEntrada = getCurrentMicroseconds();      // Iniciar t entrada
    nGenerados++;                                   // Se genera otra pieza
    sprintf(pieza.nombre, "pieza_#%d", nGenerados); // Nombramos la pieza para distinguirla
    err = put_item(pieza, &almacen1);               // Se inserta el dato en el almacen1
    if(err==-1){
      printf("ERROR: No se ha podido insertar la pieza en Almacen1\n");
    }
    sleep(TG1);                                     // Espera para generar otra pieza
  }
}

void *barnSec1 (void *args){
  pieza pieza;
  int err1, err2, err3;
  float tBarnizado;
  while(1){
    err1 = obten_valor(&pieza,&almacen1); // Obtener propiedades de la pieza
    if(err1==-1){                           // Si hay error
      printf("ERROR: No se ha podido valorar la pieza en Almacen1\n");
    }
    else{                                   // Si no hay error
      tBarnizado = (getCurrentMicroseconds()-pieza.tEntrada)/1000;  // Sacar tiempo de barnizado en seg
      if(tBarnizado<=TP1){                 // No extraer la pieza hasta pasados TP1 seg        
        err2 = get_item(&pieza,&almacen1); // Sacar las piezas
        if(err2==-1){
          printf("ERROR: No se ha podido extraer la pieza en Almacen1\n");
        }
        else{
          if(tBarnizado<TP1+TDEFBAR){       // Si se sobrepasa del tiempo maximo
            nGenerados--;                   // Descartar de nGenerados
            nDescartados++;                 // Anyadir a nDescartados
          }
          else {
            pieza.tBarnizado = tBarnizado;
          	pieza.tEntrada = getCurrentMicroseconds();                     // Si no sobrepasa el tiempo maximo
            err3 = put_item(pieza,&almacen2); // Poner piezas en almecen2
          }
        }
      }
    }
    if(err1 == -1){
      printf("ERROR: No se ha podido extraer la pieza en Almacen2\n");
    } else {
      
    }
    
  }
}

/*
  long tGeneracion;
  long tEntrada;
  int tBarnizado;
  int tSecado;
  int tCocido;
  int tTotal;
  int linea;
  char nombre[30];

void *barnizadoSecado1(void *arg){
  pieza pieza;
  float tBarnizado;

  while(1){
    sem_wait(&prod1);
    consult(&pieza,&almacen1);
    sem_post(&prod1);
    tBarnizado = (getCurrentMicroseconds()-pieza.tIN)/MS2S;
    if (tBarnizado >= tp1) {
      sem_wait(&prod1);
      get_item(&pieza,&almacen1);
      sem_post(&huecos1);
      if (tBarnizado > tp1+tmaxB){
	pthread_mutex_lock(&conEntrada_lock);
	conEntrada--;
	pthread_mutex_unlock(&conEntrada_lock);
	pthread_mutex_lock(&conDescar_lock);
	conDescar++;
	pthread_mutex_unlock(&conDescar_lock);
      }
      else{
	pieza.tBarnizado = tBarnizado;
	pieza.tIN = getCurrentMicroseconds();
	sem_wait(&huecos3);
	put_item(pieza,&almacen3);
	sem_post(&prod3);
      }
    }
    usleep(500000);
  }
}
void *generador1(){
  pieza pieza = {.generador = 1};
  while(conEntrada < PIZENUMB){
    pieza.tIN = getCurrentMicroseconds();
    pieza.tGen = pieza.tIN;
    pthread_mutex_lock(&conEntrada_lock);
    sprintf(pieza.etiqueta,"pieza%d",conEntrada);
    conEntrada++;
    pthread_mutex_unlock(&conEntrada_lock);
    sem_wait(&huecos1);
    put_item(pieza, &almacen1);
    sem_post(&prod1);
    sleep(tg1);
  }
}

void *generador2(void *arg){
  pieza pieza = {.generador = 2};

  while(conEntrada <= PIZENUMB){
    pieza.tIN = getCurrentMicroseconds();
    pieza.tGen = pieza.tIN;
    pthread_mutex_lock(&conEntrada_lock);
    sprintf(pieza.etiqueta,"pieza%d",conEntrada);
    conEntrada++;
    pthread_mutex_unlock(&conEntrada_lock);
    sem_wait(&huecos2);
    put_item(pieza, &almacen2);
    sem_post(&prod2);
    sleep(tg2);
  }
*/

int main()
{
  initbuffer(&almacen1);                     // Iniciar buffer
  initbuffer(&almacen2);                     // Iniciar buffer
  initbuffer(&almacen3);                     // Iniciar buffer
  initbuffer(&almacen4);                     // Iniciar buffer
  initbuffer(&almacen5);                     // Iniciar buffer
 
  pthread_attr_t atrib;                     //Crear atributo
 
  pthread_t p1, p2, p3, p4, p5, p6, p7, p8, pStatus; // Definir los threads

  pthread_attr_init( &atrib );              // Iniciar atributo

  pthread_create( &p1, &atrib, generaPiezas1, NULL );
//  pthread_create( &p2, &atrib, generaPiezas2, NULL );
//  pthread_create( &p3, &atrib, Barnizador1, NULL );
//  pthread_create( &p4, &atrib, Barnizador2, NULL );
//  pthread_create( &p5, &atrib, Secador1, NULL );
//  pthread_create( &p6, &atrib, Secador1, NULL );
//  pthread_create( &p7, &atrib, Cocedor1, NULL );
//  pthread_create( &p8, &atrib, Cocedor1, NULL );
  pthread_create( &pStatus, &atrib, printStatus, NULL );

//  pthread_join( p7, NULL);                   // Acabar con los threads
//  pthread_join( p8, NULL);
  pthread_join( p1, NULL);
  
  printf("FIN DE LA APLICACIÓN\n");
  return 0;
}
