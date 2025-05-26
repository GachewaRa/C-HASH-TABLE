#include <stdio.h>   // For printf, fprintf
#include <stdlib.h>  // For exit
#include <unistd.h>  // For fork, getpid
#include <sys/wait.h> // For wait

int main(int argc, char *argv[]) {
    // Declare a variable 'x' in the main (parent) process
    int x = 100;

    // Print the initial state of x in the parent process
    printf("Parent (PID: %d): Initial x = %d (Address: %p)\n", (int)getpid(), x, (void *)&x);

    // Call fork() to create a child process
    pid_t rc = fork();

    if (rc < 0) {
        // fork failed; exit with an error
        fprintf(stderr, "fork failed\n");
        exit(1);
    } else if (rc == 0) {
        // This code block is executed by the CHILD process
        printf("Child (PID: %d): Before change, x = %d (Address: %p)\n", (int)getpid(), x, (void *)&x);

        // Child changes its copy of x
        x = 200;
        printf("Child (PID: %d): After change, x = %d (Address: %p)\n", (int)getpid(), x, (void *)&x);

        // Child exits
        exit(0); // It's good practice for child processes to explicitly exit
    } else {
        // This code block is executed by the PARENT process
        // Parent waits for the child process to terminate
        int wc = wait(NULL); // wait(NULL) waits for any child to finish
        printf("Parent (PID: %d): Child %d finished (wait_rc: %d)\n", (int)getpid(), rc, wc);

        printf("Parent (PID: %d): Value of x in parent after child exited = %d (Address: %p)\n", (int)getpid(), x, (void *)&x);


        // Parent changes its copy of x
        x = 300;
        printf("Parent (PID: %d): After child finished and parent changed, x = %d (Address: %p)\n", (int)getpid(), x, (void *)&x);
    }

    return 0; // Parent process exits
}

/* Key Observations and Explanation:

Initial Value in Child:

When fork() is called, the child process gets an exact copy of the parent's memory space at the time of the fork call.
Therefore, the variable x in the child process will initially have the same value as it did in the parent 
right before fork(), which is 100.

What Happens When Both Change x:

Independent Copies: The most important takeaway is that the x in the parent and the x in the child 
are completely separate variables in separate memory spaces. They just happened to start with the same value 
because the child's memory was a copy of the parent's.

Child's Change: When the child process changes x to 200, it changes its own copy of x. This modification has 
absolutely no effect on the x variable in the parent process.

Parent's Change: Similarly, when the parent process changes x to 300, it changes its own copy of x. 
This modification has no effect on the x variable in the child (which has likely already exited, 
but even if it hadn't, its x would still be 200).

Addresses: You'll likely notice that the virtual memory addresses printed for x in both the parent 
and child processes are the same. This is because both processes have their own independent virtual 
address spaces, and the variable x is at the same offset within each of those spaces. 
However, due to Copy-On-Write (COW), these identical virtual addresses will map to different 
physical memory pages once either the parent or the child modifies the page containing x. 
The OS handles this behind the scenes.

In essence, this problem highlights the fundamental concept of process isolation: 
each process operates in its own protected memory space, and changes within one process's memory 
do not directly affect another's. */