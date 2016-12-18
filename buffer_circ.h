#include <stdio.h>
#include <stdbool.h>
#include <pthread.h>
#include "piezas.h"

#define BUFSIZE 10  // Tamaño de buff_circular
int ok;             // definir ok 

pthread_mutex_t buffer_lock = PTHREAD_MUTEX_INITIALIZER; // Iniciar mutex

struct Buffer_Circ {  // Definir estructura Buffer_Circ
  pieza buffer[BUFSIZE];
  int bufIN, bufOUT;
  int contador;
};

// Iniciar bufer
void initbuffer( struct Buffer_Circ *buff) {
  /*CON MUTEX:*/                              //-------------------------------
  pthread_mutex_lock(&buffer_lock);           // Bloquear mutex
  (*buff).bufIN = 0;
  (*buff).bufOUT = 0;
  (*buff).contador = 0;
  /*CON MUTEX:*/                         
  pthread_mutex_unlock(&buffer_lock);       // Desbloquear mutex
}

// Get item
int get_item(pieza * x, struct Buffer_Circ *buff) {
  int nxtOUT = (*buff).bufOUT % BUFSIZE;

  /*CON MUTEX:*/                              //-------------------------------
  pthread_mutex_lock(&buffer_lock);           // Bloquear mutex

  if( (*buff).contador > 0){                  // Si el buffer no esta vacio                                      
    *x = (*buff).buffer[nxtOUT];              // Asignar resultado a x
    (*buff).bufOUT = (nxtOUT + 1) % BUFSIZE;  // Actualizar bufOUT
    (*buff).contador = (*buff).contador - 1;  // Actualizar contador
   
    /*CON MUTEX:*/                            //------------------------------------
    pthread_mutex_unlock(&buffer_lock);       // Desbloquear mutex                      
    return 0;                                 // Devolver 0 -> OK
  }
  else {                                      // Si buffer esta lleno
    /*CON MUTEX:*/                         
    pthread_mutex_unlock(&buffer_lock);       // Desbloquear mutex
    return -1;                                // Devolver -1 -> NOT OK
  }
}

// Put item
int put_item(pieza x, struct Buffer_Circ *buff) {
  int nxtIN = (*buff).bufIN % BUFSIZE;

  /*CON MUTEX:*/                              //------------------------------
  pthread_mutex_lock(&buffer_lock);           // Bloquear mutex

  if( (*buff).contador < BUFSIZE ){           // Si el buffer esta vacio  

    (*buff).buffer[nxtIN] = x;                // Insertar x
    (*buff).bufIN = (nxtIN + 1) % BUFSIZE;    // Actualizar bufIN
    (*buff).contador = (*buff).contador + 1;  // Actualizar contador

    /*CON MUTEX:*/                            //-----------------------------
    pthread_mutex_unlock(&buffer_lock);       // Desbloquear mutex
    return 0;                                 // Devolver 0 -> OK
 }

  else {                                      // Si buffer esta lleno
    /*CON MUTEX:*/                            //----------------------------                        
    pthread_mutex_unlock(&buffer_lock);       // Desbloquear mutex
    return -1;                                // Devolver -1 -> NOT OK
 }
}

// Consultar si una variable Buffer_Circ está vacía
bool bc_vacio(struct Buffer_Circ *buff){
  /*CON MUTEX:*/                              //------------------------------
  pthread_mutex_lock(&buffer_lock);           // Bloquear mutex
  if( (*buff).contador == 0 ) {
    /*CON MUTEX:*/                            //----------------------------                        
    pthread_mutex_unlock(&buffer_lock);       // Desbloquear mutex
    return true;

  }
  else {
    /*CON MUTEX:*/                            //----------------------------                        
    pthread_mutex_unlock(&buffer_lock);       // Desbloquear mutex
    return false;
  }
}

// Consultar si una variable Buffer_Circ está lleno
bool bc_lleno(struct Buffer_Circ *buff){
  /*CON MUTEX:*/                              //------------------------------
  pthread_mutex_lock(&buffer_lock);           // Bloquear mutex
  if( (*buff).contador == BUFSIZE ) {
    /*CON MUTEX:*/                            //----------------------------                        
    pthread_mutex_unlock(&buffer_lock);       // Desbloquear mutex
    return true;
  }
  else {
    /*CON MUTEX:*/                            //----------------------------                        
    pthread_mutex_unlock(&buffer_lock);       // Desbloquear mutex
    return false;
  }
}

//PRINT
void print (struct Buffer_Circ *buff){
  /*CON MUTEX:*/                              //------------------------------
  pthread_mutex_lock(&buffer_lock);           // Bloquear mutex

  printf("Numero de pieza = %d\n", (*buff).contador );
  if( (*buff).contador != 0 ){                // Si hay pieza
    int i, bo=(*buff).bufOUT;
    for(i=0; i<(*buff).contador; i++){
      bo=bo%BUFSIZE;
      printf("%s (linea: %d)\n", (*buff).buffer[bo].nombre, (*buff).buffer[bo].linea );
      bo++;
    }
  }
  //printf("------------------------------------------------------------\n");
  /*CON MUTEX:*/                            //----------------------------                        
  pthread_mutex_unlock(&buffer_lock);       // Desbloquear mutex
}

// Devolver número de elementos
int num_elementos (struct Buffer_Circ *buff){
 /*CON MUTEX:*/                            //------------------------------
 pthread_mutex_lock(&buffer_lock);         // Bloquear mutex
 /*CON MUTEX:*/                            //----------------------------                        
 pthread_mutex_unlock(&buffer_lock);       // Desbloquear mutex
 return (*buff).contador;
}
 
int obten_valor (pieza *x, struct Buffer_Circ *buff){   // Obtener el último valor del buffer sin extraccion
  if( (*buff).contador == 0 ){                          // Si no hay valores
    /*CON MUTEX:*/                            //----------------------------                        
    pthread_mutex_unlock(&buffer_lock);       // Desbloquear mutex
    return -1;                                // Devolver -1 -> NOT OK
  }
  else {
    *x = (*buff).buffer[(*buff).bufOUT];
    /*CON MUTEX:*/                            //-----------------------------
    pthread_mutex_unlock(&buffer_lock);       // Desbloquear mutex
    return 0;                                 // Devolver 0 -> OK
  }
}
