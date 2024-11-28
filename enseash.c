
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main() {
    // Welcome message
    const char *welcome_message = "Welcome to ENSEA Tiny Shell.\nType 'exit' to quit.\n";
    write(STDOUT_FILENO, welcome_message, strlen(welcome_message));

    char input[1024];  // Buffer to store user input

    while (1) {
        // Display prompt
        const char *prompt = "enseash % ";
        write(STDOUT_FILENO, prompt, strlen(prompt));

        // Read user input
        ssize_t bytes_read = read(STDIN_FILENO, input, sizeof(input) - 1);
        if (bytes_read <= 0) {
            write(STDOUT_FILENO, "\nGoodbye!\n", 10);
            break;
        }

        // Null-terminate the input string and remove newline
        input[bytes_read - 1] = '\0';

        // Check for 'exit' command
        if (strcmp(input, "exit") == 0) {
            write(STDOUT_FILENO, "Goodbye!\n", 9);
            break;
        }

        // Fork a new process to execute the command
        pid_t pid = fork();
        if (pid == -1) {
            perror("fork");
            continue;
        }

        if (pid == 0) {
            // Child process: Execute the command
            execlp(input, input, NULL);
            // If execlp fails, print an error and exit the child process
            perror("execlp");
            _exit(1);
        } else {
            // Parent process: Wait for the child to complete
            int status;
            waitpid(pid, &status, 0);
        }
    }

    return 0;
}

