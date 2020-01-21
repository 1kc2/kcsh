/**
 * KC-Shell (kcsh)
 * @description: a tiny UNIX shell
 * @author: Karan Chawla
 */

/**
 * – Parses commands from standard input and executes them until a command exit is evaluated;
 * – Terminates the foreground process upon input of `^D`;
 * – Change directory using `cd` command, and display files/directories within a directory 
 *   using `ls` as well as all other built-in functions;
 * – Executes any number of processes in background (i.e., in parallel with the foreground process),
 *   including in particular, the ability to start another process while a process has been temporarily suspended;
 * – Informs the user when the background process finishes or is waiting for an input from the terminal;
 * – Informs the user what commands are executing in the background via the `jobs` command, includes
 *   information about the state of the process (i.e. suspended, background, waiting for input, etc.);
 * – Terminates involuntarily a background processes by through the `kill` command.
 */

#include "headers.h"

/*
Displays shell name on launch
 */
void welcome(void)
{
    printf("\n");
    for (int topdashes = 0; topdashes < 2; topdashes++)
    {
        for (int welcome = 0; welcome < 50; welcome++)
            printf("–");
        printf("\n");
    }
    printf("                Welcome to KC-shell\n");
    printf("     Please read the documentation before use\n");
    for (int bottomdashes = 0; bottomdashes < 2; bottomdashes++)
    {
        for (int end = 0; end < 50; end++)
            printf("–");
        printf("\n");
    }
    printf("\n");
}

/*
Search a job by process id, job id, job status
 */
processJob* getJob(int val, int parameter)
{
    usleep(10000);
    processJob* job = jobs;
    switch (parameter)
    {
    case PID:
        while (job != NULL)
        {
            if (job->pid == val)
                return job;
            else
                job = job->next;
        }
        break;
    case JID:
        while (job != NULL)
        {
            if (job->id == val)
                return job;
            else
                job = job->next;
        }
        break;
    case JOBSTATUS:
        while (job != NULL)
        {
            if (job->status == val)
                return job;
            else
                job = job->next;
        }
        break;
    default:
        return NULL;
        break;
    }
    return NULL;
}

/*
Handle signal input
 */
void sigint_handler(int signo)
{
    if (!jump)
        return;
    siglongjmp(env, 42);
}

/*
Parses input which can then be passed on to functions
 */
char **get_input(char *input)
{
    char **command = malloc(8 * sizeof(char *));
    if (command == NULL)
    {
        perror("malloc failed");
        exit(1);
    }
    
    char *separator = " ";
    char *parsed;
    int index = 0;

    parsed = strtok(input, separator);  // fragment and tokenise input to parse it 
    while (parsed != NULL)
    {
        *(command + index) = parsed;
        index++;
        parsed = strtok(NULL, separator);
    }

    *(command + index) = NULL;
    return command;
}

/*
Prints all jobs and their status
 */
void printJobs()
{
    printf("\nActive jobs:\n");
    for (int i = 0; i < 75; i++)
        printf("-");
    printf("\n");
    printf("| %7s  | %30s | %5s | %10s | %6s |\n", "job no.", "name", "pid", "desc", "status");
    for (int n = 0; n < 75; n++)
        printf("-");
    printf("\n");
    processJob* job = jobs;
    if (job == NULL)
        printf("| %s %62s |\n", "No Jobs.", "");
    else
    {
        while (job != NULL)
        {
            printf("|  %7d | %30s | %5d | %10s | %6c |\n", job->id, job->name, job->pid, job->desc, job->status);
            job = job->next;
        }
    }
    for (int j = 0; j < 75; j++)
        printf("-");
    printf("\n");
}

int main(void)
{
    char **command;
    char *input;
    pid_t child_pid;
    int stat_loc;

    // setup sigint

    struct sigaction parent;
    parent.sa_handler = sigint_handler;
    sigemptyset(&parent.sa_mask);
    parent.sa_flags = SA_RESTART;
    sigaction(SIGINT, &parent, NULL);

    welcome();
    char dir[1024];
    getcwd(dir, sizeof(dir));  // get current working directory to print
    printf("Current working directory: %s\n\n", dir);

    while (1)  // loop forever until a command exit is evaluated
    {
        if (sigsetjmp(env, 1) == 42)
        {
            write(STDOUT_FILENO, "\n", 5);
            continue;
        }
        jump = 1;
        char cwd[1024];
        getcwd(cwd, sizeof(cwd));
        char host[1024];
        gethostname(host, sizeof(host));
        printf("kcsh: %s @ %s $", cwd, host);
        input = readline(" ");  // input
        if (input == NULL)
        {
            // exit on ^D
            
            printf("\n"); 
            exit(0);
        }

        command = get_input(input);

        if (!*(command) )  // free if empty command
        {
            free(input);
            free(command);
            continue;
        }

        if (strcmp(*(command), "exit") == 0)  // "exit" command
            exit(EXIT_SUCCESS);
        
        if (*(command))

        if (strcmp(*(command), "cd") == 0)  // "cd" command
        {
            if (*(command + 1) == NULL)
                chdir(getenv("HOME"));  // returns to HOME directory if only "cd" passed in as argument
            else
            {
                if (chdir(*(command + 1)) == -1)  // go to new directory or return error if non-existent
                    perror(*(command + 1));
            }
            continue;
        }
        if (strcmp("jobs", *(command)) == 0)  // calls printJobs
            printJobs();

        if (strcmp("kill", *(command)) == 0)  // kills job
        {
            if (*(command + 1) == NULL)
                return 0;
            
            // get the job from the list

            processJob *job = getJob(atoi(*(command + 1)), JID);
            kill(job->pid, SIGKILL);  // kill job
            return 1;
        }
        child_pid = fork();  // fork to create a child process
        if (child_pid < 0)
        {
            perror("Fork failed");
            exit(1);
        }

        if (child_pid == 0)
        {
            // setup sigint

            struct sigaction child;
            child.sa_handler = sigint_handler;
            sigemptyset(&child.sa_mask);
            child.sa_flags = SA_RESTART;
            sigaction(SIGINT, &child, NULL);

            if (execvp(*(command), command) < 0)  // never returns if sys call is successful
            {
                perror(*(command));
                exit(1);
            }
        }
        else
        {
            waitpid(child_pid, &stat_loc, WUNTRACED);  // wait for process to end
        }
        if (input == NULL)
            free(input);

        if (command == NULL)
            free(command);
    }
    return 0;
}
