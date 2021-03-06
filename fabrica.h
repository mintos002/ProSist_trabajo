#ifndef FABRICA_
#define FABRICA_

#define TG1 3.0         // Definir tiempos
#define TG2 2.0
#define TP1 1.0
#define TP2 0.5
#define TP3 13.0
#define TP4 17.0
#define TP5 20.0
#define TSTATUS 1.0     // Tiempo muestra de estado
#define NUMPIEZAS 10    // Definir n piezas
#define TDEFBAR 2.0     // Tiempo (seg) barnizado defectuoso
#define TDEFSEC 4.0     // Tiempo (seg) secado defectuoso
#define TDEFCOC 6.0     // Tiempo (seg) cocido defectuoso
#define uS2S 1.0E6      // 1s a microsegundos

float tpmin=9.0E9;
float tpmax=0;
float tpsum=0;
float tpmed=0;
int nEntradas = 0;
int nSacadas = 0;
int nDescartadas = 0;

pthread_mutex_t nEntradas_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t nSacadas_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t nDescartadas_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t tiempos_mutex = PTHREAD_MUTEX_INITIALIZER;

struct Buffer_Circ almacen1, almacen2, almacen3, almacen4, almacen5; // Definir Buffer_Circ

sem_t huecos1, huecos2, huecos3, huecos4, huecos5; // Definir Semaforos
sem_t piezas1, piezas2, piezas3, piezas4, piezas5; // Definir Semaforos

long getCurrentMicroseconds();
void *printStatus(void *args);
void *productor1(void *args);
void *barnSec1 (void *args);
void *productor2(void *args);
void *barnSec2 (void *args);
void *secCoc12(void *args);
void *consumidor1(void *args);
void *consumidor2(void *args);

#endif //
