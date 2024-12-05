# TP1
Exercise 3:Management of the shell output with the command ”exit” or with <ctrl>+d;

Input Reading: The code uses read() to obtain user input, storing it in a buffer. It checks the return value to handle errors or EOF.
EOF Handling: If the user sends an EOF signal (Ctrl+D) or if there’s a read error, the program outputs a goodbye message and breaks the loop.
Exit Command: The program checks if the input command is "exit". If true, it outputs a farewell message and breaks the loop, terminating the shell.
String Comparison: strcmp() is utilized to compare the input with the "exit" command, ensuring appropriate action is taken.
Output Messages: The program uses write() to send farewell messages to the standard output, providing feedback to the user before exiting.
This structure ensures a clean exit from the shell, enhancing user experience by providing clear communication.

Exercise 4: "Display the return code (or signal) of the previous command in the prompt 

Welcome Message: The program begins by displaying a welcome message to the user.
Input Buffer: A buffer is defined to store user input commands.
Main Loop: The shell enters an infinite loop to:
Display a prompt.
Show the exit status or termination signal of the last executed command.
Read user input from standard input.
Forking: The program uses fork() to create a child process. The child attempts to execute the command with execlp(), using the full path for the kill command to ensure accuracy.
Error Handling: If execlp() fails, an error message is printed, and the child process exits.
Waiting for Child Process: The parent process waits for the child to terminate and retrieves its exit status using waitpid().

Exercise 5: "Measurement of the command execution time using the call clock_gettime 

Process Creation: The program creates a child process using fork() which allows it to execute commands while the parent process can manage and track the execution time.

Command Execution: The child process uses execlp() to execute the command specified by the user. This function replaces the current process image with a new process image.

Handling Special Commands: There is special handling for the "kill" command to ensure it is executed with its full path. This avoids potential issues if the command is not found in the environment's PATH.

Error Handling: If execlp() fails, an error message is printed using perror(), and the child process exits with a failure status. This is important for debugging and ensuring that errors are reported correctly.

Time Measurement: The parent process uses clock_gettime() with CLOCK_MONOTONIC to measure the execution time of the child process. This is more reliable for measuring elapsed time than using time() because it is not subject to system clock changes.

Exit Status Handling: After waiting for the child process to finish, the parent checks whether the child exited normally or was terminated by a signal. It uses macros like WIFEXITED() and WIFSIGNALED() to determine the nature of the termination.

Output Formatting: The execution time and the exit status (or signal number) are printed in a formatted manner to the standard output, providing feedback to the user in a clear format.

Return Value: The program ends with a return value of 0, indicating successful execution of the main function. This is a common practice in C programming to signal success.

Excercise 6:Execution of a complex command (with arguments) ;

Command Parsing: The program successfully parses input commands into a command and its arguments using strtok(), allowing for flexible command execution.

Process Creation: It utilizes fork() to create a new process. The child process executes the command using execlp() or execvp(), while the parent process waits for the child to complete.

Error Handling: The implementation includes error handling for command execution failures, providing users with feedback through perror().

Exit Mechanism: The program allows users to exit the shell by typing 'exit', enhancing user experience.

Execution Status: The prompt displays the exit status of the last executed command, giving users insights into whether their commands were successful or not.

Missing Package: It is noted that the osfortune package is not found in the application, which means that users will not be able to utilize this feature for displaying system fortunes.

Exercise 7 :"Management of redirections to stdin and stdout with ’<’ and ’>’ ;

