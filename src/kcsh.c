#include "headers.h"
 
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
