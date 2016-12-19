#ifndef BUFFER_CIRC_
#define BUFFER_CIRC_

#define BUFSIZE 10
int ok; // definir ok 

struct Buffer_Circ *buff;
void initbuffer (struct Buffer_Circ *buff);
int get_item (pieza* x, struct Buffer_Circ *buff);
int put_item (pieza x, struct Buffer_Circ *buff);
bool bc_vacio (struct Buffer_Circ *buff);
bool bc_lleno (struct Buffer_Circ *buff);
void print (struct Buffer_Circ *buff);
int num_elementos (struct Buffer_Circ *buff);
int obten_valor (pieza *x, struct Buffer_Circ *buff);

pthread_mutex_t buffer_lock = PTHREAD_MUTEX_INITIALIZER; // Iniciar mutex

#endif //
