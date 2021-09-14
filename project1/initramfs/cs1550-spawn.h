#ifndef CS1550_SPAWN_H
#define CS1550_SPAWN_H

#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

void spawn(char *name)
{
        char *const argv[] = { name, NULL };
        char *const envp[] = { NULL };
        pid_t child;
        int status;

        child = fork();

        // Error occurred
        if (child < 0) {
                perror("fork");
                return;
        }

        // Parent process
        if (child > 0) {
                waitpid(child, &status, 0);

                if (status != 0) {
                        printf("%s returned exit status %d\n", name, status);
                }

                return;
        }

        // Child process
        if (execve(name, argv, envp) < 0)
                perror("execve");
}


#endif // CS1550_SPAWN_H
