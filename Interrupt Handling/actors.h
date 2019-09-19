#ifndef ACT_PRINT_H
#define ACT_PRINT_H
#include "fifo.h"
#include <stdio.h>
#include <assert.h>

void actor_mul(fifo_t *F1, fifo_t *F2, fifo_t *q);
void actor_increment(fifo_t *F1, fifo_t *q);
void actor_fork(fifo_t *F1, fifo_t *q1, fifo_t *q2);
void actor_print(fifo_t *F);

#endif
