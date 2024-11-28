#include <stdio.h>
#include <string.h>
#include <unistd.h>

int main() {
    // Welcome message
    const char *welcome_message = "Congratulations! You just solved exercise 1.\nType 'exit' to quit.\n";
    write(STDOUT_FILENO, welcome_message, strlen(welcome_message));

    // Shell loop
    char input[1024];
    while (1) {
        // Display prompt
        const char *prompt = "enseash % ";
        write(STDOUT_FILENO, prompt, strlen(prompt));

        // Read user input
        ssize_t bytes_read = read(STDIN_FILENO, input, sizeof(input) - 1);

        // Handle errors or EOF
        if (bytes_read <= 0) {
            write(STDOUT_FILENO, "\nGoodbye!\n", 10);
            break;
        }

        // Null-terminate the input string
        input[bytes_read - 1] = '\0';

        // Check if the user wants to exit
        if (strcmp(input, "exit") == 0) {
            write(STDOUT_FILENO, "Goodbye!\n", 9);
            break;
        }
    }

    return 0;
}

