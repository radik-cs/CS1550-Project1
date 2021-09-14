/*
 * A simple test program for Project 1's signal() and wait() syscalls.
 * The output of this program should always start with
 *     This line has to print first
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

	if (fork() == 0) {
		// First child
		// Give a chance for the second child to perform wait() first (hopefully!)
		sleep(1);
		cs1550_down(sem_id);

		fprintf(stderr, "This line has to print last\n");

		cs1550_up(sem_id);
	} else if (fork() == 0) {
		// Second child
		cs1550_down(sem_id);

		fprintf(stderr, "This line has to print second\n");
	} else {
		// Parent
		fprintf(stderr, "This line has to print first\n");

		// Now, both child processes should be waiting on the semaphore.
		// Unblock the second one...
		cs1550_up(sem_id);
		sleep(1);

		// Unblock the first one...
		cs1550_up(sem_id);

		// Clean up
		wait(NULL);
		wait(NULL);
		cs1550_close(sem_id);
	}

	return 0;
}
