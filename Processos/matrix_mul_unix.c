
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>

int fork_test();

int main(int argc, char** argv)
{
	fork_test();
}

typedef struct tuple_struct
{

	int a;
	int b;

} tuple;

/*
int multiply(int** m1, int** m2, int a, int b, int c, int d) // m1: a x b matrix, m2: c x d matrix
{
	if(a < 1 || b < 1 || c < 1 || d < 1)
	{
		fprintf(stderr, "Expected 2 matrices m1: a x b, m2: c x d where a,b,c,d >= 1");
		exit(1);
	}

	if(b != c)
	{
		fprintf(stderr,"The second dimension of the first matrix must match the first dimension of the second matrix\n");
		exit(2);
	}

	int num_processes = a*d;

	tuple* line_column_pairs = (tuple*)malloc(num_processes*sizeof(tuple));

	int i; int j;

	for(i = 0; i < a; i++)
	{
		for(j = 0; j < d; j++)
		{
			line_column_pairs[i*d + j] = (tuple) {.a = i, .b = j};
		}
	}

	int** m3 = (int**)malloc(sizeof(int))
}
*/

int fork_test()
{
	int numberOfChildren = 10;
	pid_t *childPids = NULL;
	pid_t p;
	
	/* Allocate array of child PIDs: error handling omitted for brevity */
	childPids = malloc(numberOfChildren * sizeof(pid_t));
	
	/* Start up children */
	for (int ii = 0; ii < numberOfChildren; ++ii) 
	{
	   if ((p = fork()) == 0)
	   {
	   		// Child process: do your work here

	   		fprintf(stderr, "Process %d says hello!\n", ii);

	   		exit(0);
	   }
	   else 
	   {
	      childPids[ii] = p;
	   }
	}
	
	/* Wait for children to exit */
	int stillWaiting;
	do 
	{
		stillWaiting = 0;

	    for (int ii = 0; ii < numberOfChildren; ++ii) 
	    {
	       if (childPids[ii] > 0) 
	       {
	          if (waitpid(childPids[ii], NULL, WNOHANG) == 0) 
	          {
	             /* Child is done */
	             childPids[ii] = 0;
	          }
	          else 
	          {
	             /* Still waiting on this child */
	             stillWaiting = 1;
	          }
	       }
	       /* Give up timeslice and prevent hard loop: this may not work on all flavors of Unix */
	       sleep(0);
	    }
	} while (stillWaiting);
	
	/* Cleanup */
	free(childPids);
}