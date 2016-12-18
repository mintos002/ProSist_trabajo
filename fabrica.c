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

#define TG1 3.0         // Definir tiempos
#define TG2 2.0
#define TP1 5.0
#define TP2 4.0
#define TP3 10.0
#define TP4 15.0
#define TP5 13.0
#define TSTATUS 1.0
#define NUMPIEZAS 12  // Definir n piezas
#define TDEFBAR 2.0     // Tiempo (seg) barnizado defectuoso
#define TDEFSEC 4.0     // Tiempo (seg) secado defectuoso
#define TDEFCOC 6.0     // Tiempo (seg) cocido defectuoso

#define uS2S 1.0E6

int nEntradas = 0;
int nSacadas = 0;
int nDescartadas = 0;

pthread_mutex_t nEntradas_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t nSacadas_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t nDescartadas_mutex = PTHREAD_MUTEX_INITIALIZER;

struct Buffer_Circ almacen1, almacen2, almacen3, almacen4, almacen5; // Definir Buffer_Circ

sem_t huecos1, huecos2, huecos3, huecos4, huecos5; // Definir Semaforos
sem_t piezas1, piezas2, piezas3, piezas4, piezas5; // Definir Semaforos

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

void *generaPiezas1(void *args){                    // Generar piezas cada TG1 seg
  pieza pieza;
  int err;
  while(nEntradas < NUMPIEZAS){                     // Mientras queden piezas por generar
    pieza.linea = 1;                                // La pieza es de la linea 1
    pieza.tNacido = getCurrentMicroseconds();       // Iniciar t nacido
    pieza.tEntrada = getCurrentMicroseconds();      // Iniciar t entrada
    pthread_mutex_lock(&nEntradas_mutex);            // Bloquear zona critica
    nEntradas++;                                    // Se genera otra pieza
    sprintf(pieza.nombre, "pieza_#%d", nEntradas);  // Nombramos la pieza para distinguirla
    
    pthread_mutex_unlock(&nEntradas_mutex);          // Desbloqueamos zona critica   
    sem_wait(&huecos1);                             // Esperar el hueco
    if(bc_lleno(&almacen1)){}
    err = put_item(pieza, &almacen1);               // Se inserta el dato en el almacen1
    if(err == -1){
      printf("ERROR: No se ha podido insertar la pieza en Almacen1\n");
      sem_post(&piezas1);                           // Postear pieza
    }
    else{
      sem_post(&piezas1);                           // Postear pieza
    }
    sleep(TG1);                                     // Espera para generar otra pieza
  }//end_while
}

void *barnSec1 (void *args){
  pieza pieza;
  int err1, err2, err3;
  float tBarnizado;
  while(1){
    sem_wait(&piezas1);                               // Esperar a que haya pieza    
    err1 = obten_valor(&pieza,&almacen1);             // Obtener propiedades de la pieza    
    if(err1==-1){                                      // Si no hay error
      printf("ERROR: No se ha podido valorar la pieza en Almacen1\n");     
      sem_post(&piezas1);                             // Postear pieza
    }
    else{
      sem_post(&piezas1);                             // Postear pieza   
      tBarnizado = (getCurrentMicroseconds()-pieza.tEntrada)/uS2S;  // Sacar tiempo de barnizado en seg
      if(tBarnizado>=TP1){                            // No extraer la pieza hasta pasados TP1 seg     
        sem_wait(&piezas1);                           // Esperar piezas1
        err2 = get_item(&pieza,&almacen1);            // Sacar las pieza 
        if(err2==-1){
          printf("ERROR: No se ha podido extraer la pieza en Almacen1\n");
          sem_post(&huecos1);                         // Postear huecos2
        }
        else{       
          sem_post(&huecos1);                           // Postear huecos1
          if(tBarnizado>TP1+TDEFBAR){                 // Si se sobrepasa del tiempo maximo   
            pthread_mutex_lock(&nEntradas_mutex);     // Bloquear zona critica       
            nEntradas--;                              // Descartar de nEntradas
            pthread_mutex_unlock(&nEntradas_mutex);   //Desbloquear zona critica
            pthread_mutex_lock(&nDescartadas_mutex);  // Bloquear zona critica
            nDescartadas++;                           // Anyadir a nDescartadas
            pthread_mutex_unlock(&nDescartadas_mutex);//Desbloquear zona critica
          }
          else{
            pieza.tBarnizado = tBarnizado;            // Anyadir tBarnizado a pieza
            pieza.tEntrada = getCurrentMicroseconds();
            sem_wait(&huecos3);                       // Esperar a que haya hueco en almazen3
            err3 = put_item(pieza,&almacen3);         // Poner piezas en almecen2
            if(err3==-1){
              printf("ERROR: No se ha podido insertar la pieza en Almacen3\n");
              sem_post(&piezas3);                     // Postear piezas3
            }
            else{
              sem_post(&piezas3);
            }
          }//end_err3
        }//end_err2
      }//end_err1
    }
    usleep(500000);    
  }//end_while
}

void *generaPiezas2(void *args){                    // Generar piezas cada TG2 seg
  pieza pieza;
  int err;
  while(nEntradas < NUMPIEZAS){                     // Mientras queden piezas por generar
    pieza.linea = 2;                                // La pieza es de la linea 1
    pieza.tNacido = getCurrentMicroseconds();       // Iniciar t nacido
    pieza.tEntrada = getCurrentMicroseconds();      // Iniciar t entrada
    pthread_mutex_lock(&nEntradas_mutex);           // Bloquear zona critica 
    nEntradas++;                                    // Se genera otra pieza
    sprintf(pieza.nombre, "pieza_#%d", nEntradas);  // Nombramos la pieza para distinguirla 
    pthread_mutex_unlock(&nEntradas_mutex);         // Desbloquear zona critica
    sem_wait(&huecos2);                             // Esperar el hueco 
    err = put_item(pieza, &almacen2);               // Se inserta el dato en el almacen1
    if(err==-1){
      printf("ERROR: No se ha podido insertar la pieza en Almacen2\n");    
      sem_post(&piezas2);                           // Postear pieza
    }
    else{
      sem_post(&piezas2);                           // Postear pieza
    }
    sleep(TG2);                                     // Espera para generar otra pieza
  }
}

void *barnSec2 (void *args){
  pieza pieza;
  int err1, err2, err3;
  float tBarnizado;
  while(1){
    sem_wait(&piezas2);                               // Esperar a que haya pieza    
    err1 = obten_valor(&pieza,&almacen2);             // Obtener propiedades de la pieza    
    if(err1==-1){                                     // Si hay error
      printf("ERROR: No se ha podido valorar la pieza en Almacen2\n");     
      sem_post(&piezas2);                             // Postear pieza
    }
    else{                                             // Si no hay error
      sem_post(&piezas2);                             // Postear pieza   
      tBarnizado = (getCurrentMicroseconds()-pieza.tEntrada)/uS2S;  // Sacar tiempo de barnizado en seg
      if(tBarnizado>=TP2){                            // No extraer la pieza hasta pasados TP1 seg     
        sem_wait(&piezas2);                           // Esperar piezas1
        err2 = get_item(&pieza,&almacen2);            // Sacar las pieza
        if(err2==-1){
          printf("ERROR: No se ha podido extraer la pieza en Almacen2\n");
          sem_post(&huecos2);                         // Postear huecos2
        }
        else{
          sem_post(&huecos2);                         // Postear huecos2
          if(tBarnizado>TP2+TDEFBAR){                 // Si se sobrepasa del tiempo maximo   
            pthread_mutex_lock(&nEntradas_mutex);     // Bloquear zona critica       
            nEntradas--;                              // Descartar de nEntradas
            pthread_mutex_unlock(&nEntradas_mutex);   //Desbloquear zona critica
            pthread_mutex_lock(&nDescartadas_mutex);  // Bloquear zona critica
            nDescartadas++;                           // Anyadir a nDescartadas
            pthread_mutex_unlock(&nDescartadas_mutex);//Desbloquear zona critica
          }
          else {
            pieza.tBarnizado = tBarnizado;            // Anyadir tBarnizado a pieza
            pieza.tEntrada = getCurrentMicroseconds();
          	sem_wait(&huecos3);                       // Esperar a que haya hueco en almazen3
            err3 = put_item(pieza,&almacen3);         // Poner piezas en almecen2
            if(err3==-1){
              printf("ERROR: No se ha podido insertar la pieza en Almacen3\n");
              sem_post(&piezas3);                     // Postear piezas3
            }
            else{
              sem_post(&piezas3);                     // Postear piezas3
            }
          }
        }
      }//if
    }//end_err1
    usleep(500000);    
  }//end_while
}

int main()
{
  initbuffer(&almacen1);                     // Iniciar buffer
  initbuffer(&almacen2);                     // Iniciar buffer
  initbuffer(&almacen3);                     // Iniciar buffer
  initbuffer(&almacen4);                     // Iniciar buffer
  initbuffer(&almacen5);                     // Iniciar buffer
 
  sem_init(&huecos1,0,BUFSIZE);
  sem_init(&huecos2,0,BUFSIZE);
  sem_init(&huecos3,0,BUFSIZE);
  sem_init(&huecos4,0,BUFSIZE);
  sem_init(&huecos5,0,BUFSIZE);
  sem_init(&piezas1,0,0);
  sem_init(&piezas2,0,0);
  sem_init(&piezas3,0,0);
  sem_init(&piezas4,0,0);
  sem_init(&piezas5,0,0);
 
  pthread_attr_t atrib;                     //Crear atributo
 
  pthread_t p1, p2, p3, p4, p5, p6, p7, p8, pStatus; // Definir los threads

  pthread_attr_init( &atrib );              // Iniciar atributo

//  pthread_create( &p1, &atrib, generaPiezas1, NULL );
  pthread_create( &p2, &atrib, generaPiezas2, NULL );
//  pthread_create( &p3, &atrib, barnSec1, NULL );
  pthread_create( &p4, &atrib, barnSec2, NULL );
//  pthread_create( &p5, &atrib, Secador1, NULL );
//  pthread_create( &p6, &atrib, Secador1, NULL );
//  pthread_create( &p7, &atrib, Cocedor1, NULL );
//  pthread_create( &p8, &atrib, Cocedor1, NULL );
  pthread_create( &pStatus, &atrib, printStatus, NULL );

//  pthread_join( p7, NULL);                   // Acabar con los threads
//  pthread_join( p8, NULL);
  pthread_join(p2,NULL);
  pthread_join(p4,NULL);
//  pthread_join( p3, NULL);
  
  printf("FIN DE LA APLICACIÓN\n");
  return 0;
}
