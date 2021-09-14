#ifndef CS1550_SYSCALL_H
#define CS1550_SYSCALL_H

#include <asm/unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

/*
 * Define a helpful debug wrapper which will print an error and halt things
 * if something goes wrong with the syscall execution.
 */

long syscall_wrapper(int number, const char *name, long value)
{
	long result = syscall(number, value);

	if (result < 0) {
		perror(name);
		exit(1);
	}

	return result;
}

long cs1550_create(long value)
{
        return syscall_wrapper(__NR_cs1550_create, "cs1550_create", value);
}

long cs1550_down(long sem_id)
{
        return syscall_wrapper(__NR_cs1550_down, "cs1550_down", sem_id);
}

long cs1550_up(long sem_id)
{
        return syscall_wrapper(__NR_cs1550_up, "cs1550_up", sem_id);
}

long cs1550_close(long sem_id)
{
        return syscall_wrapper(__NR_cs1550_close, "cs1550_close", sem_id);
}

#endif // CS1550_SYSCALL_H
