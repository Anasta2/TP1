#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <time.h>

#define MAX_ARGS 100  // Maximum number of arguments

int main() {
    // Welcome message
    const char *welcome_message = "Congratulations! You just solved exercise 1.\nType 'exit' to quit.\n";
    write(STDOUT_FILENO, welcome_message, strlen(welcome_message));

    char input[1024];  // Buffer to store user input
    int last_status = 0;  // To store the status of the last command

    while (1) {
        // Display prompt with return code, signal, and execution time
        if (WIFEXITED(last_status)) {
            dprintf(STDOUT_FILENO, "enseash [exit:%d] %% ", WEXITSTATUS(last_status));
        } else if (WIFSIGNALED(last_status)) {
            dprintf(STDOUT_FILENO, "enseash [sign:%d] %% ", WTERMSIG(last_status));
        } else {
            write(STDOUT_FILENO, "enseash % ", 10);
        }

        // Read user input
        ssize_t bytes_read = read(STDIN_FILENO, input, sizeof(input) - 1);

        // Handle <Ctrl>+D (EOF) or read error
        if (bytes_read <= 0) {
            write(STDOUT_FILENO, "\nBye bye...\n", 12);
            break;
        }

        // Null-terminate the input string and remove newline
        input[bytes_read - 1] = '\0';

        // Check for 'exit' command
        if (strcmp(input, "exit") == 0) {
            write(STDOUT_FILENO, "Bye bye...\n", 11);
            break;
        }

        // Parse the input into command and arguments
        char *args[MAX_ARGS] = {NULL};
        int arg_count = 0;

        char *token = strtok(input, " ");
        while (token != NULL && arg_count < MAX_ARGS - 1) {
            args[arg_count++] = token;
            token = strtok(NULL, " ");
        }
        args[arg_count] = NULL;  // Null-terminate the argument list

        // Fork a new process to execute the command
        pid_t pid = fork();
        if (pid == -1) {
            perror("fork");
            continue;
        }

        if (pid == 0) {
            // Child process: Execute the command with arguments
            execvp(args[0], args);
            // If execvp fails, print an error and exit the child process
            perror("execvp");
            _exit(1);
        } else {
            // Parent process: Measure execution time and wait for the child to complete
            struct timespec start, end;
            clock_gettime(CLOCK_MONOTONIC, &start);  // Record start time

            waitpid(pid, &last_status, 0);

            clock_gettime(CLOCK_MONOTONIC, &end);  // Record end time
            long exec_time_ms = (end.tv_sec - start.tv_sec) * 1000 +
                                (end.tv_nsec - start.tv_nsec) / 1000000;

            // Update prompt with execution time
            if (WIFEXITED(last_status)) {
                dprintf(STDOUT_FILENO, "enseash [exit:%d|%ldms] %% ", WEXITSTATUS(last_status), exec_time_ms);
            } else if (WIFSIGNALED(last_status)) {
                dprintf(STDOUT_FILENO, "enseash [sign:%d|%ldms] %% ", WTERMSIG(last_status), exec_time_ms);
            }
        }
    }

    return 0;
}

