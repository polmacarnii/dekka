#include <arch.h>
#include <proc.h>

/* tabla de procesos y proceso actual */
static Proc procs[NPROC];
Proc *up;

/* peticion de cambio de contexto pendiente, es levantada por el timer */
volatile int need_resched;

/* El ultimo indice elegido por el Round-Robin, de ahí en adelante se busca el
 * siguiente listo para intentar repartir el CPU de manera equitativa */
static int rr_last;

/* Busca un descriptor libre en la tabla o devuelve NULL si esta lleno */
static Proc *allocproc(void) {
  for (int i = 0; i < NPROC; i++) {
    if (procs[i].state == Free) {
      Proc *p = &procs[i];
      p->sp = NULL;
      p->chan = NULL;
      p->next = NULL;
      p->entry = NULL;
      return p;
    }
  }
  return NULL;
}

/* Es la funcion que corre un proceso recien inicia, se entra a
 * traves de "ret" final de swtch(), asi que no debe retornar nada,
 * si la funcion de entrada muere el proceso muere con kexit() */

/* Nota que venimos con interrupciones deshabilitadas porue el cambio de
 * contexto se origino dentro de una IRQ(y un gate de interrupcion limpian *IF),
 * asi que hay que re-habilitarlas o en caso contrario la preemptibilidad se
 * pierde*/
static void forkret(void) {
  interrupts_enable();

  if (up->entry != NULL) {
    up->entry();
  }

  kexit();
}

/* Esta funcion fabrica la pila inicial de un proceso para que el primer swtch()
 * hacia el "retorne" dentro de forkret() */
static uchar *init_stack(Proc *p) {
  uintptr_t top = (uintptr_t)(p->kstack + KSTACK);

  /* La pila crece hacia abajo y arranca alineada a 16 bytes */
  top &= ~(uintptr_t)0xF;

  uint32_t *frame = (uint32_t *)(top - 5 * sizeof(uint32_t));

  frame[0] = 0;                            /* edi */
  frame[1] = 0;                            /* esi */
  frame[2] = 0;                            /* ebx */
  frame[3] = 0;                            /* ebp */
  frame[4] = (uint32_t)(uintptr_t)forkret; /* direccion de retorno */

  return (uchar *)frame;
}

/* Elige el siguiente proceso listo en round-robin y se salta el actual, el
 * indice 0 no se elige aqui nunca, se pone como ultimo recurso */
static Proc *pick_next(Proc *skip) {
  for (int i = 1; i <= NPROC; i++) {
    int idx = (rr_last + i) % NPROC;

    if (idx == 0) {
      continue; /* procs[0] = idle, reservado */
    }
    if (&procs[idx] == skip) {
      continue;
    }

    if (procs[idx].state == Ready) {
      return &procs[idx];
    }
  }

  return NULL;
}

/* API Publica */

void proc_init(void) {
  for (int i = 0; i < NPROC; i++) {
    procs[i].sp = NULL;
    procs[i].state = Free;
    procs[i].chan = NULL;
    procs[i].next = NULL;
    procs[i].entry = NULL;
  }

  /* procs[0] representa el contexto desde el que arranca el kernel (kmain).
   * Hace de idle y nunca muere */
  procs[0].state = Running;
  up = &procs[0];

  rr_last = 0;
  need_resched = 0;
}

Proc *kthread_create(void (*entry)(void)) {
  if (entry == NULL)
    return NULL;

  Proc *p = allocproc();
  if (p == NULL) {
    return NULL;
  }

  p->entry = entry;
  p->sp = init_stack(p);
  p->state = Ready;

  return p;
}

void sched(void) {
  Proc *prev = up;
  Proc *next = pick_next(prev);

  if (next == NULL) {
    /* no hay otro proceso listo, si ya somos el idle no hay a donde ir, si no,
     * le devolvemos el CPU al idle, ya que este siempre esta disponible */
    if (prev == &procs[0])
      return;

    next = &procs[0];
    rr_last = 0;
  } else {
    rr_last = (int)(next - procs);
  }

  next->state = Running;
  up = next;

  /* Guarda el contexto de prev y salta al de next y cando a prev le toque
   * otra vez, swtch() retorna aqui y sigue */
  swtch(&prev->sp, next->sp);

  /* Al reanudarnos, otro proceso nos eligio y dejo up apuntandonos */
  prev->state = Running;
}

void yield(void) {
  up->state = Ready;
  sched();
}

void kexit(void) {
  /* Un proceso free no vuelve a ser elegido por pick_next(), por ende, swtch()
   * no retornara */
  up->state = Free;
  sched();

  /* Inalcanzable pero por si acaso, mejor prevenir que lamentar */
  for (;;)
    halt();
}

void sched_tick(void) {
  if (need_resched) {
    need_resched = 0;
    yield();
  }
}
