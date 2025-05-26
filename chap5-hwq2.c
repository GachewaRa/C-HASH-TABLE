#include <stdio.h>    // For printf, fprintf, perror
#include <stdlib.h>   // For exit
#include <unistd.h>   // For fork, getpid, open, close, write
#include <sys/wait.h> // For wait
#include <fcntl.h>    // For open flags (O_CREAT, O_WRONLY, O_TRUNC)
#include <string.h>   // For strlen

int main(int argc, char *argv[]) {
    // 1. Open a file before calling fork()
    // O_CREAT: Create the file if it doesn't exist
    // O_WRONLY: Open for writing only
    // O_TRUNC: Truncate (empty) the file if it already exists
    // 0644: Permissions (rw-r--r--)
    int fd = open("output.txt", O_CREAT | O_WRONLY | O_TRUNC, 0644);
    if (fd < 0) {
        perror("open failed"); // Print error message for open() failure
        exit(1);
    }

    printf("Parent (PID: %d): File opened with descriptor %d\n", (int)getpid(), fd);

    // Call fork() to create a child process
    pid_t rc = fork();

    if (rc < 0) {
        // fork failed; exit with an error
        perror("fork failed"); // Use perror for system call errors
        exit(1);
    } else if (rc == 0) {
        // This code block is executed by the CHILD process
        printf("Child (PID: %d): In child process, file descriptor is %d\n", (int)getpid(), fd);

        // Child writes to the file
        char *child_msg = "Child writing.\n";
        write(fd, child_msg, strlen(child_msg));
        write(fd, child_msg, strlen(child_msg)); // Write again to show interleaving

        // Child exits
        exit(0);
    } else {
        // This code block is executed by the PARENT process
        // Parent waits for the child process to terminate
        int wc = wait(NULL); // wait(NULL) waits for any child to finish
        printf("Parent (PID: %d): Child %d finished (wait_rc: %d)\n", (int)getpid(), rc, wc);

        // Parent writes to the file
        char *parent_msg = "Parent writing.\n";
        write(fd, parent_msg, strlen(parent_msg));
        write(fd, parent_msg, strlen(parent_msg)); // Write again to show interleaving

        // Close the file descriptor in the parent
        close(fd);
        printf("Parent (PID: %d): File descriptor %d closed.\n", (int)getpid(), fd);
    }

    return 0;
}


/* Explanation:

File Descriptor Inheritance:

When open("output.txt", ...) is called in the parent, the kernel creates an "open file description" 
(a data structure in the kernel that holds information about the file, including its current offset) 
and gives the parent a file descriptor (an integer, e.g., 3) that points to this description.

When fork() is called, the child process receives a copy of the parent's file descriptor table. 
This means the child also gets a file descriptor 3, and this 3 also points to the exact same 
open file description in the kernel as the parent's file descriptor 3.

Shared File Offset:

Because both file descriptors (parent's fd=3 and child's fd=3) point to the same open file description, 
they share the same file offset.
This means if the parent writes 10 bytes, the file offset advances by 10. 
If the child then writes, it will start writing from that new offset, not from the beginning of the file.

Concurrent Writes and Interleaving:

The parent and child processes run concurrently after fork(). The operating system's scheduler decides 
which process gets to run on the CPU at any given moment.

When both processes call write(fd, ...), they are both trying to write to the same underlying file 
and are both using and advancing the same shared file offset.

Since there's no synchronization mechanism (like a lock or semaphore) in place, the writes 
from the parent and child can happen in any order. 
The scheduler might let the child write a bit, then switch to the parent to write, then back to the child, and so on. 
This leads to the interleaved output you'll observe in output.txt.

In essence, this problem demonstrates that while processes have isolated memory spaces, 
they can share access to underlying system resources like files, and when they do so 
concurrently without coordination, the results can be non-deterministic. 
This is a crucial concept for understanding concurrency and the need for synchronization in operating systems. */