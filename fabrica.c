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
#include "buffer_circ.c"
#include "piezas.c"



struct Buffer_Circ almacen1, almacen2, almacen3, almacen4, almacen5; // Definir Buffer_Circ

sem_t mutex, items, huecos;           // Definir semaforos

long getCurrentMicroseconds(){        // Obtener tiempo
	struct timespec currentTime;
	clock_gettime(CLOCK_MONOTONIC, &currentTime);
	return (currentTime.tv_sec)*1000000 + (currentTime.tv_nsec) / 1000;
}

void *GeneradorP1(void *args){
  err = put_item(almacen1);
}

int main()
{
  initbuffer(&almacen1);                     // Iniciar buffer
  initbuffer(&almacen2);                     // Iniciar buffer
  initbuffer(&almacen3);                     // Iniciar buffer
  initbuffer(&almacen4);                     // Iniciar buffer
  initbuffer(&almacen5);                     // Iniciar buffer
 
  pthread_attr_t atrib;                     //Crear atributo
 
  pthread_t p1, p2, p3, p4, p5, p6, p7, p8; // Crear los threads

  pthread_attr_init( &atrib );              // Iniciar atributo

  pthread_create( &p1, &atrib, GeneradorP1, NULL );
  pthread_create( &p2, &atrib, GeneradorP2, NULL );
  pthread_create( &p3, &atrib, Barnizador1, NULL );
  pthread_create( &p4, &atrib, Barnizador2, NULL );
  pthread_create( &p5, &atrib, Secador1, NULL );
  pthread_create( &p6, &atrib, Secador1, NULL );
  pthread_create( &p7, &atrib, Cocedor1, NULL );
  pthread_create( &p8, &atrib, Cocedor1, NULL );

  pthread_join( p7, NULL);                   // Acabar con los threads
  pthread_join( p8, NULL);

  printf("FIN DE LA APLICACIÓN\n");
  return 0;
}
