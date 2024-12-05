# TP1

Casapu Irina
Anastasiu Rares-Ioan

**Exercise 1: Display a welcome message, followed by a simple prompt.**

For this exercise we have 2 codes : Display+update enseash.c
Welcome Message: Both codes start by displaying a welcome message to inform the user they have solved exercise 1 and can type 'exit' to quit.
User Input Loop: They enter an infinite loop, prompting the user with enseash % for command input.
Reading Input: User input is read using the read function, with error handling for EOF or input errors.
Command Execution:
If the command is not "exit", a new process is forked. The child process executes the command using execlp.
The program specifically checks for the "kill" command and attempts to execute it with its full path. Errors in command execution are reported with perror.
Process Management: The parent process waits for the child to finish, retrieves the exit status, and displays the return code or signal from the last command.
Exit Handling: If "exit" is typed, the program prints a goodbye message and exits the loop, terminating the shell.
In the second code (enseash.c), there’s a minor redundancy in the welcome message declaration, but it follows the same structure as the first snippet regarding user input, command execution, and termination

**Exercise 2 : Execution of the entered command and return to the prompt (REPL : read–eval–print
loop) :**
a) read the command entered by user,
b) execute this command (simple command for the moment, without argument)),
c) print the prompt enseash % and waits for a new command

Welcome Message: The shell displays a message informing users they can exit by typing "exit."
Shell Loop: A continuous loop (while (1)) runs until the user exits.
Prompt Display: The prompt "enseash %" indicates readiness for user commands.
Reading User Input: User input is read using the read system call into a buffer, with error checking for EOF or other conditions.
Processing Input: The input string is null-terminated, with trailing newlines removed. If the input is "exit," a goodbye message is displayed, and the loop breaks.
Command Execution: For other commands, a new process is created using fork(). The child process executes the command with execlp(), and errors are reported if execution fails.
Waiting for Child Process: The parent process uses waitpid() to wait for the child process to finish before prompting for new commands.Forking: fork() creates a new child process.
Executing Commands: execlp() executes commands found in the system’s PATH.
Process Synchronization: waitpid() ensures the parent waits for the child’s completion.
String Handling: strcmp() compares strings, and string manipulation is used to process user input properly.

**Exercise 3:Management of the shell output with the command ”exit” or with <ctrl>+d;**

Input Reading: The code uses read() to obtain user input, storing it in a buffer. It checks the return value to handle errors or EOF.
EOF Handling: If the user sends an EOF signal (Ctrl+D) or if there’s a read error, the program outputs a goodbye message and breaks the loop.
Exit Command: The program checks if the input command is "exit". If true, it outputs a farewell message and breaks the loop, terminating the shell.
String Comparison: strcmp() is utilized to compare the input with the "exit" command, ensuring appropriate action is taken.
Output Messages: The program uses write() to send farewell messages to the standard output, providing feedback to the user before exiting.
This structure ensures a clean exit from the shell, enhancing user experience by providing clear communication.

**Exercise 4: "Display the return code (or signal) of the previous command in the pro**mpt 

Welcome Message: The program begins by displaying a welcome message to the user.
Input Buffer: A buffer is defined to store user input commands.
Main Loop: The shell enters an infinite loop to:
Display a prompt.
Show the exit status or termination signal of the last executed command.
Read user input from standard input.
Forking: The program uses fork() to create a child process. The child attempts to execute the command with execlp(), using the full path for the kill command to ensure accuracy.
Error Handling: If execlp() fails, an error message is printed, and the child process exits.
Waiting for Child Process: The parent process waits for the child to terminate and retrieves its exit status using waitpid().

**Exercise 5: "Measurement of the command execution time using the call clock_gettime** 

Process Creation: The program creates a child process using fork() which allows it to execute commands while the parent process can manage and track the execution time.

Command Execution: The child process uses execlp() to execute the command specified by the user. This function replaces the current process image with a new process image.

Handling Special Commands: There is special handling for the "kill" command to ensure it is executed with its full path. This avoids potential issues if the command is not found in the environment's PATH.

Error Handling: If execlp() fails, an error message is printed using perror(), and the child process exits with a failure status. This is important for debugging and ensuring that errors are reported correctly.

Time Measurement: The parent process uses clock_gettime() with CLOCK_MONOTONIC to measure the execution time of the child process. This is more reliable for measuring elapsed time than using time() because it is not subject to system clock changes.

Exit Status Handling: After waiting for the child process to finish, the parent checks whether the child exited normally or was terminated by a signal. It uses macros like WIFEXITED() and WIFSIGNALED() to determine the nature of the termination.

Output Formatting: The execution time and the exit status (or signal number) are printed in a formatted manner to the standard output, providing feedback to the user in a clear format.

Return Value: The program ends with a return value of 0, indicating successful execution of the main function. This is a common practice in C programming to signal success.

**Excercise 6:Execution of a complex command (with arguments) ;**

Command Parsing: The program successfully parses input commands into a command and its arguments using strtok(), allowing for flexible command execution.

Process Creation: It utilizes fork() to create a new process. The child process executes the command using execlp() or execvp(), while the parent process waits for the child to complete.

Error Handling: The implementation includes error handling for command execution failures, providing users with feedback through perror().

Exit Mechanism: The program allows users to exit the shell by typing 'exit', enhancing user experience.

Execution Status: The prompt displays the exit status of the last executed command, giving users insights into whether their commands were successful or not.

Missing Package: It is noted that the osfortune package is not found in the application, which means that users will not be able to utilize this feature for displaying system fortunes.

**Exercise 7 :"Management of redirections to stdin and stdout with ’<’ and ’>’ ;**
Redirection: Demonstrates handling input/output redirection with < and > operators for file operations.
Process Management: Uses fork() to create a child process for command execution, allowing the parent to continue running.
Error Handling: Includes error handling for file operations and command execution, ensuring predictable behavior.
Argument Parsing: Utilizes strtok to dynamically parse command-line input, accommodating varying argument numbers.
Execution Timing: Measures command execution time with clock_gettime, useful for performance analysis.
Multiple Commands: Provides a foundation for supporting multiple commands and enhancing shell functionality.
Signal Handling: Checks the child process's exit status, reporting normal exits or terminations by signals for debugging purposes.

**Exercise 8 : Management of pipe redirection with ‘|’ :**

The provided C code is an implementation of a simple shell that supports command execution with features such as piping, input/output redirection, and execution time measurement.
The shell can handle commands separated by the pipe (|) operator, allowing the output of one command to be used as the input to another.
It creates a pipe using the pipe() system call, forks two child processes for the commands, and sets up the necessary redirections with dup2
Input and Output Redirection:
The shell supports input redirection using < and output redirection using >.
It uses open() to manage file descriptors for the redirected input and output.
Execution Time Measurement:
The shell measures the execution time of commands using the clock_gettime() function, providing feedback on how long a command takes to execute
Error Handling:
The code includes error handling for system calls like pipe(), fork(), and execvp(), ensuring that the shell can report failures gracefully.
This code represents a solid foundation for a shell program in C, demonstrating key concepts in process management, inter-process communication, and system calls. 


