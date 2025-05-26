#define _GNU_SOURCE // Define _GNU_SOURCE to enable GNU extensions, including execvpe

#include <stdio.h>    // For printf, fprintf, perror
#include <stdlib.h>   // For exit, getenv
#include <unistd.h>   // For fork, getpid, exec* functions
#include <sys/wait.h> // For wait
#include <string.h>   // For strdup

// Function to run a specific exec variant in a child process
void run_exec_variant(const char *variant_name, int variant_id) {
    pid_t rc = fork();

    if (rc < 0) {
        // fork failed
        perror("fork failed");
        _exit(1); // Use _exit in child to avoid flushing parent's buffers
    } else if (rc == 0) {
        // Child process
        printf("Child (PID: %d) attempting %s...\n", (int)getpid(), variant_name);

        // Define arguments for /bin/ls.
        // Conventionally, argv[0] is the program name itself.
        char *ls_args[] = {"ls", "-l", NULL}; // For 'v' variants

        // Define a custom environment for 'e' variants
        char *custom_env[] = {"MY_CUSTOM_VAR=HelloFromExec", "PATH=/bin:/usr/bin", NULL};

        switch (variant_id) {
            case 1: // execl: list of args, full path
                execl("/bin/ls", "ls", "-l", NULL);
                break;
            case 2: // execle: list of args, full path, custom environment
                execle("/bin/ls", "ls", "-l", NULL, custom_env);
                break;
            case 3: // execlp: list of args, searches PATH
                execlp("ls", "ls", "-l", NULL); // "ls" will be found via PATH
                break;
            case 4: // execv: array of args, full path
                execv("/bin/ls", ls_args);
                break;
            case 5: // execvp: array of args, searches PATH
                execvp("ls", ls_args); // "ls" will be found via PATH
                break;
            case 6: // execvpe: array of args, searches PATH, custom environment
                execvpe("ls", ls_args, custom_env);
                break;
            default:
                fprintf(stderr, "Unknown exec variant ID: %d\n", variant_id);
                _exit(1);
        }

        // If exec* succeeds, this line is NEVER reached.
        // If it fails, print an error and exit the child.
        perror("exec failed"); // Print specific error for exec failure
        _exit(1); // Child must exit on exec failure
    } else {
        // Parent process
        int status;
        pid_t child_pid = waitpid(rc, &status, 0); // Wait for this specific child
        if (child_pid == -1) {
            perror("waitpid failed");
            exit(1);
        }
        printf("Parent (PID: %d): Child %d (%s) finished with status %d\n\n",
               (int)getpid(), child_pid, variant_name, WEXITSTATUS(status));
    }
}

int main(int argc, char *argv[]) {
    printf("Parent (PID: %d): Starting exec() variant tests.\n", (int)getpid());

    // Run each variant sequentially
    run_exec_variant("execl()", 1);
    run_exec_variant("execle()", 2);
    run_exec_variant("execlp()", 3);
    run_exec_variant("execv()", 4);
    run_exec_variant("execvp()", 5);
    run_exec_variant("execvpe()", 6);

    printf("Parent (PID: %d): All exec() variant tests completed.\n", (int)getpid());

    return 0;
}

/*Why So Many Variants of the Same Basic Call?
The existence of six (and sometimes more) exec() variants might seem redundant at first, 
but they provide flexibility and cater to different programming needs and historical contexts:

Argument Passing Style (l vs. v):

l (list): Functions like execl, execle, execlp take arguments as a variable-length list of strings, 
terminated by a NULL pointer. This is often convenient for a fixed, small number of arguments known at compile time.

v (vector): Functions like execv, execve, execvp, execvpe take arguments as a null-terminated array (vector) of 
string pointers (char *const argv[]). This is more flexible when the number of arguments is dynamic or comes 
from another array (e.g., main's argv).

Environment Handling (e suffix):

Functions without the e suffix (execl, execv, execlp, execvp) inherit the calling process's environment variables. 
This is the most common behavior.

Functions with the e suffix (execle, execve, execvpe) allow you to explicitly provide a custom environment for 
the new program as a null-terminated array of strings. This is useful when you need to control the environment precisely 
for the new process, perhaps to hide certain variables or set specific ones.

Path Resolution (p suffix):

Functions without the p suffix (execl, execle, execv, execve) require the full (absolute or relative) path to the 
executable program. If the path is incorrect, exec will fail.

Functions with the p suffix (execlp, execvp, execvpe) will search the directories specified in the PATH environment 
variable to find the executable. This is convenient because you don't need to know the exact path to common utilities 
like ls, grep, cat, etc.*/