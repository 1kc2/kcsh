# kcsh

A tiny UNIX shell written in C.

---
# Features

* Parses commands from standard input and executes them until a command exit is evaluated;
  
* Terminates the foreground process upon input of `^D`;

* Change directory usind `cd` command, and display files/directories within a directory using `ls`;

* Executes any number of processes in background (i.e., in parallel with the foreground process), including in particular, the ability to start another process while a process has been temporarily suspended;

* Informs the user when the background process finishes or is waiting for an input from the terminal;

* Informs the user what commands are executing in the background via the `jobs` command, includes information about the state of the process (i.e. suspended, background, waiting for input, etc.);

* Terminates involuntarily a background processes by through the `kill` command.

# How to compile and run

1. Open Terminal/Command Prompt.

2. Navigate to the `src` directory containing the `kcsh.c` file (and the `headers.h` file).

3. Compile the shell program using the following command:

    `gcc -g -lreadline kcsh.c -o kcsh`

4. Run the shell by typing `./kcsh` (or simply `kcsh` on Windows) and pressing enter.
