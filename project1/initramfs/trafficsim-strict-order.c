/*
 * A simple test program for Project 1's signal() and wait() syscalls.
 * The output of this program should always be:
 *     This line has to print first
 *     This line has to print second
 *     This line has to print last
 * no matter how long the processes sleep.
 */

#include <stdio.h>
#include <sys/mman.h>
#include <sys/resource.h>
#include <sys/wait.h>
#include <unistd.h>

#include "cs1550-syscall.h"

int main()
{
	long sem_id = cs1550_create(0);
	long nextsem_id = cs1550_create(0);

	if (fork() == 0) {
		// First child
		cs1550_down(sem_id);

		fprintf(stderr, "This line has to print second\n");

		cs1550_up(nextsem_id);
	} else if (fork() == 0) {
		// Second child
		cs1550_down(nextsem_id);

		fprintf(stderr, "This line has to print last\n");
	} else {
		// Parent

		// Now, both child processes should be waiting.
		// Parent process keeps going...
		fprintf(stderr, "This line has to print first\n");

		// Unblock the first child
		cs1550_up(sem_id);

		// Clean up
		wait(NULL);
		wait(NULL);
		cs1550_close(sem_id);
		cs1550_close(nextsem_id);
	}

	return 0;
}
