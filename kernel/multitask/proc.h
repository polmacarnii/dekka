#ifndef PROC_H
#define PROC_H

#include <types.h>

/* Estados de un proceso, solo puede haber uno corriendo a la vez, sleep es que
 * esta bloqueado esperando por up->chan */
enum { Free, Ready, Running, Sleeping };

/* NPROC: tamano de la tabla de procesos */
/* KSTACK: tamano de la pila de kernel de cada proceso*/
enum { NPROC = 16, KSTACK = 4096 };

typedef struct Proc Proc;

/* descriptor de hilo del kernel, todo en ring 0 por ahora */
struct Proc {
    uchar *sp;            /* puntero de pila guardado por swtch */
    int state;            /* uno de los estados de arriba */
    void *chan;           /* canal por el que duerme (reservado) */
    Proc *next;           /* siguiente en la cola (reservado) */
    void (*entry)(void);  /* punto de entrada del hilo */
    uchar kstack[KSTACK]; /* pila de kernel del proceso */
};

extern Proc *up;

extern volatile int need_resched;

void proc_init(void);
Proc *kthread_create(void (*entry)(void));
void sched(void);
void yield(void);
void kexit(void);
void sched_tick(void);

#endif
