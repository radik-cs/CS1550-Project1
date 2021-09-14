/*
 * A simple test program for Project 1's signal() and wait() syscalls.
 * Entering and exiting the critical section should never be
 * preempted.
 */

#include <math.h>
#include <stdio.h>
#include <sys/mman.h>
#include <sys/resource.h>
#include <sys/wait.h>
#include <unistd.h>

#include "cs1550-syscall.h"

#define LOOPS1 1 // workload for each process
#define LOOPS2 1 // number of iterations

int main()
{
        int i, j;
        float m = 1.0;

        // Create a semaphore and initialize it to 1, to simulate a mutex
        long sem_id = cs1550_create(1);

	if (fork() == 0) {
		// First child
		for (i = 0; i < LOOPS2; i++) {
			cs1550_down(sem_id);
			fprintf(stderr, "Child 1 process entering critical section, iteration %i.\n", i);

			for (j = 0; j < LOOPS1; j++) {
				m = m*log(j*i*149384);
				m = exp(m);
			}

			fprintf(stderr, "Child 1 process exiting CS, iteration %i. j=%i\n", i, j);
			cs1550_up(sem_id);
		}
	} else if (fork() == 0) {
		// Second child
		for (i = 0; i < LOOPS2; i++) {
			cs1550_down(sem_id);
			fprintf(stderr, "Child 2 process entering critical section, iteration %i.\n", i);

			for (j = 0; j < LOOPS1; j++) {
				m = m*log(j*i*149384);
				m = exp(m);
			}

			fprintf(stderr, "Child 2 process exiting CS, iteration %i. j=%i\n", i, j);
			cs1550_up(sem_id);
		}
	} else {
		// Parent
		for (i = 0; i < LOOPS2; i++) {
			cs1550_down(sem_id);
			fprintf(stderr, "parent process entering critical section, iteration %i.\n", i);

			for (j = 0; j < LOOPS1; j++) {
				m = m*log(j*i*149384);
				m = exp(m);
			}

			fprintf(stderr, "parent process exiting CS, iteration %i. j=%i\n", i, j);
			cs1550_up(sem_id);
		}

		// Clean up
		wait(NULL);
		wait(NULL);
		cs1550_close(sem_id);
	}

	return 0;
}
