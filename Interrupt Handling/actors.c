#include "actors.h"

void actor_mul(fifo_t *F1, fifo_t *F2, fifo_t *q)
{
	assert(F1 != 0);
	assert(F2 != 0);
	assert(q != 0);
	// firing rule: fire if there's at least one token on each of the inputs, F1 and F2
	if ((fifo_size(F1) > 0) && (fifo_size(F2) > 0))
		put_fifo(q, get_fifo(F1) * get_fifo(F2));
}

void actor_increment(fifo_t *F1, fifo_t *q)
{
	assert(F1 != 0);
	assert(q != 0);
	// firing rule: fire if there's at least one token on input F1
	if ((fifo_size(F1) > 0))
		put_fifo(q, (get_fifo(F1) + 1));
}

void actor_fork(fifo_t *F1, fifo_t *q1, fifo_t *q2)
{
	assert(F1 != 0);
	assert(q1 != 0);
	assert(q2 != 0);

	int temp;
	temp = get_fifo(F1);

	put_fifo(q1, temp);
	put_fifo(q2, temp);
}

void actor_print(fifo_t *F)
{
	assert(F!=0);
	//firing rule: fire if there's at least one token in F
	if (fifo_size(F) > 0)
		printf("%d\n", get_fifo(F)); 
}

