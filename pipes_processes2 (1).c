#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>
#include <sys/wait.h>

int main() {
    // Create two pipes for two-way communication
    int fd1[2]; // Pipe from parent to child
    int fd2[2]; // Pipe from child to parent

    char fixed_str1[] = "howard.edu";
    char fixed_str2[] = "gobison.org";
    char input_str[100];
    char buffer[100];
    pid_t pid;

    // Create the pipes
    if (pipe(fd1) == -1 || pipe(fd2) == -1) {
        perror("Pipe Failed");
        return 1;
    }

    pid = fork();

    if (pid < 0) {
        perror("fork Failed");
        return 1;
    } else if (pid > 0) { // Parent process (P1)
        // Close unused ends of the pipes
        close(fd1[0]);  // Close reading end of fd1
        close(fd2[1]);  // Close writing end of fd2

        // Prompt for the input string
        printf("Enter a string to concatenate (P1): ");
        scanf("%s", input_str);

        // Send the string to the child (P2)
        write(fd1[1], input_str, strlen(input_str) + 1);
        close(fd1[1]); // Close writing end after sending data

        // Wait for the child to send data
        wait(NULL);

        // Receive the concatenated string from the child
        read(fd2[0], buffer, sizeof(buffer)); 
        printf("Received from child (P1): %s\n", buffer); // Print the first string

        // Concatenate with fixed_str2 and print
        strcat(buffer, fixed_str2);
        printf("Final result in Parent: %s\n", buffer);

        // Close reading end of fd2
        close(fd2[0]);

    } else { // Child process (P2)
        // Close unused ends of the pipes
        close(fd1[1]); // Close writing end of fd1
        close(fd2[0]); // Close reading end of fd2

        // Read a string using the first pipe
        read(fd1[0], input_str, sizeof(input_str));
        close(fd1[0]); // Close reading end after receiving data

        // Concatenate "howard.edu" and print
        strcat(input_str, fixed_str1);
        printf("Concatenated string (P2): %s\n", input_str);

        // Send the concatenated string back to the parent
        write(fd2[1], input_str, strlen(input_str) + 1);
        
        // Prompt for a second input
        printf("Enter a second string to concatenate (P2): ");
        scanf("%s", input_str);

        // Send the second string to the parent
        write(fd2[1], input_str, strlen(input_str) + 1);
        
        close(fd2[1]); // Close writing end after sending data

        exit(0);
    }

    return 0;
}