#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <readline/readline.h>
#include <sys/wait.h>
#include <setjmp.h>
#define PID 1
#define JID 2
#define JOBSTATUS 3

void welcome(void)  // displays shell name on launch
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

static sigjmp_buf env;  // declaring buffer for signal handling
static volatile sig_atomic_t jump = 0;

typedef struct job  // custom struct to define a "job" akin to a linked list – "struct job * next"
{
    int id;
    char *name;
    int status;
    char *desc;
    pid_t pid;
    struct job *next;
} processJob;

static processJob* jobs = NULL;

processJob* getJob(int val, int parameter)  // search a job by process id, job id, job status
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

void sigint_handler(int signo)  // handle signal input
{
    if (!jump)
        return;
    siglongjmp(env, 42);
}

char **get_input(char *input)  // parsing input which is then passed on to functions
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

void printJobs()  // prints all jobs and their status
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
