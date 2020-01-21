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

void welcome(void);

static sigjmp_buf env;  // declaring buffer for signal handling

static volatile sig_atomic_t jump = 0;

typedef struct job  // custom struct to define a "job" 
{
    int id;
    char *name;
    int status;
    char *desc;
    pid_t pid;
    struct job *next;
} processJob;

static processJob* jobs = NULL;

processJob* getJob(int val, int parameter);

void sigint_handler(int signo);

char **get_input(char *input);

void printJobs();
