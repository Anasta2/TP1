# TP1
Exercise1: Display a welcome message, followed by a simple prompt.

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

Exercise2 : Execution of the entered command and return to the prompt (REPL : read–eval–print
loop) :
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

Excercise3.  Management of the shell output with the command ”exit” or with <ctrl>+d;




