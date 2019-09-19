#include "fifo.h"
#include "actors.h"

int main()
{
	fifo_t input1;
	fifo_t input2;
	init_fifo(&input1);
	init_fifo(&input2);

	fifo_t add1;
	fifo_t add2;
	init_fifo(&add1);
	init_fifo(&add2);

	fifo_t val1;
	init_fifo(&val1);

	fifo_t val2;
	init_fifo(&val2);

	fifo_t intermediate1;
	fifo_t intermediate2;
	init_fifo(&intermediate1);
	init_fifo(&intermediate2);

	fifo_t output1;
	init_fifo(&output1);

	// create two initial tokens with value 24 and 1
	put_fifo(&input1, 24);
	put_fifo(&input2, 1);

	//initial fork
	actor_fork(&input1, &intermediate1, &intermediate2);
	actor_fork(&input2, &add1, &add2);

	unsigned int i = 0;
	// iterate the system schedule 100 times
	for (i = 0; i < 100; i++)
	{
		//multiply input 1 by input 2 
		actor_mul(&add1,&intermediate2,&output1);

		//increment input 2
		actor_increment(&add2, &val1);

		//print multiple
		actor_print(&output1);

		//setup for next iteration
		int temp_loop; 
		temp_loop = get_fifo(&intermediate1);
		put_fifo(&val2, temp_loop);

		actor_fork(&val2, &intermediate1, &intermediate2);
		actor_fork(&val1, &add1, &add2);
	}
	// after this completes, the output1 queue
	// will contain a single token with value 100
	return 0;
}
