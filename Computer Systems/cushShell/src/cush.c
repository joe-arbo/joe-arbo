/*
 * cush - the customizable shell.
 *
 * Developed by Godmar Back for CS 3214 Summer 2020 
 * Virginia Tech.  Augmented to use posix_spawn in Fall 2021.
 */
#define _GNU_SOURCE    1
#include <stdio.h>
#include <readline/readline.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include <sys/wait.h>
#include <assert.h>
#include <limits.h>
#include <fcntl.h>
#include <readline/history.h>

/* Since the handed out code contains a number of unused functions. */
#pragma GCC diagnostic ignored "-Wunused-function"

#include "termstate_management.h"
#include "signal_support.h"
#include "shell-ast.h"
#include "utils.h"
#include "spawn.h"

static void handle_child_status(pid_t pid, int status);

static void
usage(char *progname)
{
    printf("Usage: %s -h\n"
        " -h            print this help\n",
        progname);

    exit(EXIT_SUCCESS);
}

/* Build a prompt */
static char *
build_prompt(void)
{
    int promptSize = 256;
    char prompt[promptSize];
    int bufSize = 64;

    char username[bufSize];
    if (getlogin_r(username, sizeof username) != 0){ //add username to prompt
        printf("Null user!\n");
        exit(1);
    }

    char hostname[bufSize];
    if (gethostname(hostname, sizeof hostname) != 0){ //add hostname to prompt
        printf("Null hostname!\n");
        exit(1);
    }
    const char* dot = ".";
    char* hostToken = strtok(hostname, dot);

    char cwd[PATH_MAX];
    if (getcwd(cwd, sizeof cwd) == NULL){//add cwd to prompt
        printf("Null CWD!\n");
        exit(1);
    }
    char* pathToken = strrchr(cwd, '/');

    snprintf(prompt, promptSize, "<%s@%s %s>$ ", username, hostToken, pathToken+1); //build prompt
    return strdup(prompt);
}

enum job_status {
    FOREGROUND,     /* job is running in foreground.  Only one job can be
                       in the foreground state. */
    BACKGROUND,     /* job is running in background */
    STOPPED,        /* job is stopped via SIGSTOP */
    NEEDSTERMINAL,  /* job is stopped because it was a background job
                       and requires exclusive terminal access */
};

struct pid_list{
    struct list_elem elem; //Link element for pid list.
    pid_t pid; //Value of pid
};

struct job {
    struct list_elem elem;   /* Link element for jobs list. */
    struct ast_pipeline *pipe;  /* The pipeline of commands this job represents */
    int     jid;             /* Job id. */
    enum job_status status;  /* Job status. */ 
    int  num_processes_alive;   /* The number of processes that we know to be alive */
    struct termios saved_tty_state;  /* The state of the terminal when this job was 
                                        stopped after having been in foreground */
    //struct list pid_list; //Points to list of pids associated with job
    pid_t pid;

    /* Add additional fields here if needed. */
};

/* Utility functions for job list management.
 * We use 2 data structures: 
 * (a) an array jid2job to quickly find a job based on its id
 * (b) a linked list to support iteration
 */
#define MAXJOBS (1<<16)
static struct list job_list;

static struct job * jid2job[MAXJOBS];

/* Return job corresponding to jid */
static struct job * 
get_job_from_jid(int jid)
{
    if (jid > 0 && jid < MAXJOBS && jid2job[jid] != NULL)
        return jid2job[jid];

    return NULL;
}

/* Return job corresponding to pid */
static struct job *
get_job_from_pid(pid_t pid){
    struct job* foundJob;

    for (int i = 1; i < MAXJOBS; ++i){ //for each POSSIBLE job
        if (get_job_from_jid(i) != NULL){ //if job exists
            foundJob = get_job_from_jid(i);

            //struct list_elem *e;

            // for (e = list_begin (&foundJob->pid_list); e != list_end (&foundJob->pid_list);
            //     e = list_next (e)){ //for each element in pid_list
            //         struct pid_list *f = list_entry (e, struct pid_list, elem);
            //         if (f->pid == pid){
            //             return foundJob;
            //         }
            //     }
            // }
            if (foundJob->pid == pid){
                return foundJob;
            }
        }
    }
    return NULL; //default case / not found
}

/* Add a new job to the job list */
static struct job *
add_job(struct ast_pipeline *pipe)
{
    //copy pipe somewhere else
    //struct ast_pipeline *newPipe = malloc(sizeof(struct ast_pipeline));
    //memcpy(newPipe, pipe, sizeof(struct ast_pipeline));

    struct job * job = malloc(sizeof *job);
    job->pipe = pipe;
    job->num_processes_alive = 0;
    job->status = pipe->bg_job;
    list_push_back(&job_list, &job->elem);
    //list_init(&job->pid_list);//new
    for (int i = 1; i < MAXJOBS; i++) {
        if (jid2job[i] == NULL) {
            jid2job[i] = job;
            job->jid = i;
            return job;
        }
    }
    fprintf(stderr, "Maximum number of jobs exceeded\n");
    abort();
    return NULL;
}

/* Delete a job.
 * This should be called only when all processes that were
 * forked for this job are known to have terminated.
 */
static void
delete_job(struct job *job)
{
    int jid = job->jid;
    assert(jid != -1);
    jid2job[jid]->jid = -1;
    jid2job[jid] = NULL;
    ast_pipeline_free(job->pipe);
    free(job);
}

static const char *
get_status(enum job_status status)
{
    switch (status) {
    case FOREGROUND:
        return "Foreground";
    case BACKGROUND:
        return "Running";
    case STOPPED:
        return "Stopped";
    case NEEDSTERMINAL:
        return "Stopped (tty)";
    default:
        return "Unknown";
    }
}

/* Print the command line that belongs to one job. */
static void
print_cmdline(struct ast_pipeline *pipeline)
{
    struct list_elem * e = list_begin (&pipeline->commands); 
    for (; e != list_end (&pipeline->commands); e = list_next(e)) {
        struct ast_command *cmd = list_entry(e, struct ast_command, elem);
        if (e != list_begin(&pipeline->commands))
            printf("| ");
        char **p = cmd->argv;
        printf("%s", *p++);
        while (*p)
            printf(" %s", *p++);
    }
}

/* Print a job */
static void
print_job(struct job *job)
{
    printf("[%d]\t%s\t\t(", job->jid, get_status(job->status));
    print_cmdline(job->pipe);
    printf(")\n");
}

/*
 * Suggested SIGCHLD handler.
 *
 * Call waitpid() to learn about any child processes that
 * have exited or changed status (been stopped, needed the
 * terminal, etc.)
 * Just record the information by updating the job list
 * data structures.  Since the call may be spurious (e.g.
 * an already pending SIGCHLD is delivered even though
 * a foreground process was already reaped), ignore when
 * waitpid returns -1.
 * Use a loop with WNOHANG since only a single SIGCHLD 
 * signal may be delivered for multiple children that have 
 * exited. All of them need to be reaped.
 */
static void
sigchld_handler(int sig, siginfo_t *info, void *_ctxt)
{
    pid_t child;
    int status;

    assert(sig == SIGCHLD);

    while ((child = waitpid(-1, &status, WUNTRACED|WNOHANG)) > 0) {
        handle_child_status(child, status);
    }
}

/* Wait for all processes in this job to complete, or for
 * the job no longer to be in the foreground.
 * You should call this function from a) where you wait for
 * jobs started without the &; and b) where you implement the
 * 'fg' command.
 * 
 * Implement handle_child_status such that it records the 
 * information obtained from waitpid() for pid 'child.'
 *
 * If a process exited, it must find the job to which it
 * belongs and decrement num_processes_alive.
 *
 * However, note that it is not safe to call delete_job
 * in handle_child_status because wait_for_job assumes that
 * even jobs with no more num_processes_alive haven't been
 * deallocated.  You should postpone deleting completed
 * jobs from the job list until when your code will no
 * longer touch them.
 *
 * The code below relies on `job->status` having been set to FOREGROUND
 * and `job->num_processes_alive` having been set to the number of
 * processes successfully forked for this job.
 */
static void
wait_for_job(struct job *job)
{
    assert(signal_is_blocked(SIGCHLD));

    while (job->status == FOREGROUND && job->num_processes_alive > 0) {
        int status;
        //printf("Triggered\n");
        pid_t child = waitpid(-1, &status, WUNTRACED);

        // When called here, any error returned by waitpid indicates a logic
        // bug in the shell.
        // In particular, ECHILD "No child process" means that there has
        // already been a successful waitpid() call that reaped the child, so
        // there's likely a bug in handle_child_status where it failed to update
        // the "job" status and/or num_processes_alive fields in the required
        // fashion.
        // Since SIGCHLD is blocked, there cannot be races where a child's exit
        // was handled via the SIGCHLD signal handler.
        if (child != -1)
        {
            handle_child_status(child, status);
        }
        else
            utils_fatal_error("waitpid failed, see code for explanation");
    }
}



static void
handle_child_status(pid_t pid, int status)
{
    assert(signal_is_blocked(SIGCHLD));

    /* To be implemented. 
     * Step 1. Given the pid, determine which job this pid is a part of
     *         (how to do this is not part of the provided code.)
     * Step 2. Determine what status change occurred using the
     *         WIF*() macros.
     * Step 3. Update the job status accordingly, and adjust 
     *         num_processes_alive if appropriate.
     *         If a process was stopped, save the terminal state.
     */
   struct job* thisJob = get_job_from_pid(pid);

   if (thisJob == NULL){
       printf("get_job_from_pid returned NULL which should not be possible!\n");
       exit(1);
   }

    if (WIFSTOPPED(status)){//stopped
        if (WSTOPSIG(status) == SIGTSTP){//User stops fg process with Ctrl-Z; bash prints something out, should probably print the same thing
            thisJob->status = STOPPED;
            termstate_save(&thisJob->saved_tty_state);
            printf("\n");
            print_job(thisJob);
        }
        else if (WSTOPSIG(status) == SIGSTOP){//User stops process with kill -STOP
            thisJob->status = STOPPED;
            termstate_save(&thisJob->saved_tty_state);
        }
        else if (WSTOPSIG(status) == SIGTTOU || WSTOPSIG(status) == SIGTTIN){//non-fg process wants terminal access
            thisJob->status = STOPPED;
            termstate_save(&thisJob->saved_tty_state);
        }
    }
    else if (WIFEXITED(status)){//dead
        thisJob->num_processes_alive--;
    }
    else if (WIFSIGNALED(status)){//dead
        if (WTERMSIG(status) == SIGINT){//user terminates process with Ctrl-C
            thisJob->num_processes_alive--;
        }
        else if (WTERMSIG(status) == SIGTERM){//user terminates process with kill
            printf("terminated\n");
            thisJob->num_processes_alive--;
        }
        else if (WTERMSIG(status) == SIGKILL){//user terminates process with kill -9
            thisJob->num_processes_alive--;
            printf("killed\n");
        }
        
        /*this is for 'general cases', which include signals from die.c
        */
        else if (WTERMSIG(status)){//general process termination
            thisJob->num_processes_alive--;
            if (WTERMSIG(status) == SIGFPE){
                printf("floating point exception\n");
            }
            else if (WTERMSIG(status) == SIGSEGV){
                printf("segmentation fault\n");
            }
            else if (WTERMSIG(status) == SIGABRT){
                printf("aborted\n");
            }
        }
    }
}

extern char **environ;

int
main(int ac, char *av[])
{
    int opt;

    /* Process command-line arguments. See getopt(3) */
    while ((opt = getopt(ac, av, "h")) > 0) {
        switch (opt) {
        case 'h':
            usage(av[0]);
            break;
        }
    }

    list_init(&job_list);
    signal_set_handler(SIGCHLD, sigchld_handler);
    termstate_init();

    using_history();

    /* Read/eval loop. */
    for (;;) 
    {
        //for posix_spawn setup
        pid_t pid;
        int status = 0;
        posix_spawn_file_actions_t action;
        int maxfd = 1024;
        char *tty_name = ttyname(0);
        
        /* Do not output a prompt unless shell's stdin is a terminal */
        char * prompt = isatty(0) ? build_prompt() : NULL;
        char * cmdline = readline(prompt);
        free (prompt);

        if (cmdline == NULL)  /* User typed EOF */
            break;

        //add line to history
        char history_to_add[strlen(cmdline) + 1];
        snprintf(history_to_add, strlen(cmdline) + 1, "%s", cmdline);
        add_history(history_to_add);


        struct ast_command_line * cline = ast_parse_command_line(cmdline);
        free (cmdline);
        if (cline == NULL)                  /* Error in command line */
            continue;

        if (list_empty(&cline->pipes)) {    /* User hit enter */
            ast_command_line_free(cline);
            continue;
        }

        //retrieve the commands parsed by the parser
        //ast_command_line_print(cline);
        struct ast_pipeline *inputPipe = NULL;
        for (struct list_elem * e = list_begin (&cline->pipes); e != list_end (&cline->pipes); e = list_next (e)) 
        {
            inputPipe = list_entry(e, struct ast_pipeline, elem);

            //sample termstate before child process spawns
            termstate_sample();

            struct ast_command *inputCmd = NULL;

            for (struct list_elem * e = list_begin(&inputPipe->commands); 
                    e != list_end(&inputPipe->commands); 
                    e = list_next(e)) 
                    {
                        inputCmd = list_entry(e, struct ast_command, elem);
                    }

            //count the number of commands parsed
            char **p = inputCmd->argv;
            char **pTemp = p;
            int commandCount = 0;
            while(*pTemp)
            {
                pTemp++;
                ++commandCount;
            }

            //form array holding the commands
            char *argsv[commandCount + 1];
            int count = 0;
            while (count < commandCount)
            {
                argsv[count] = *p++;
                ++count;
            }
            argsv[count] = NULL;

            //history builtin
            if (strcmp(argsv[0], "history") == 0){
                HIST_ENTRY** histlist = history_list();
                
                for (int i = 0; i < history_length; ++i){
                    printf("%d\t%s\n", i+1, histlist[i]->line);
                }
                continue;
            }

            //exit builtin. invoked as 'exit' or 'exit N' where N is desired status
            if (strcmp(argsv[0], "exit") == 0)
            {
                if (argsv[1] != NULL)
                {
                    int parsedStatus = atoi(argsv[1]);
                    printf("exit\r\n");
                    exit(parsedStatus);
                }
                else
                {
                    printf("exit\r\n");
                    exit(status);
                }
            }
            
            //jobs builtin. invoked as 'jobs'
            if (strcmp(argsv[0], "jobs") == 0)
            {
                struct job* thisJob;
                for (int i = 1; i < MAXJOBS; ++i)
                {
                    thisJob = get_job_from_jid(i);
                    //note: to prevent iteration through all of MAXJOBS, could
                    //implement a break if it runs to a certain amount of nulls in a row...
                    //might be conceptually hard to pin down though
                    if (thisJob != NULL) 
                    {
                        print_job(thisJob);
                    }
                }
                continue;
            }
            
            //kill builtin. invoked as 'kill N' where N is jid to kill
            if (strcmp(argsv[0], "kill") == 0){ 
                //secondword will be jid of process to kill
                int thisJid = atoi(argsv[1]);
                struct job* thisJob = get_job_from_jid(thisJid);
                kill(thisJob->pid, SIGTERM);
                continue;
            }
            
            //stop builtin. invoked as 'stop N' where N is jid to stop
            if (strcmp(argsv[0], "stop") == 0){ 
                //secondword will be jid of process to stop
                int thisJid = atoi(argsv[1]);
                struct job* thisJob = get_job_from_jid(thisJid);
                kill(thisJob->pid, SIGTSTP);
                thisJob->status = STOPPED;
                continue;
            }
            
            //bg builtin. invoked by 'bg N' where N is jid to put in background
            if (strcmp(argsv[0], "bg") == 0)
            {
                struct job* thisJob;
                int thisJid = atoi(argsv[1]);
                thisJob = get_job_from_jid(thisJid);
                
                if (thisJob->status == STOPPED)
                {
                    thisJob->status = BACKGROUND;
                    kill(thisJob->pid, SIGCONT);
                    printf("[%d]  ", thisJid);
                    print_cmdline(thisJob->pipe);
                    printf(" &\n");
                }
                else if (thisJob->status == BACKGROUND)
                {
                    printf("bg: job %d already in background\n", thisJid);
                }
                continue;
            }

            //fg builtin. invoked as "fg N" where N is jid to put in foreground
            if (strcmp(argsv[0], "fg") == 0)
            {
                struct job* thisJob;
                int thisJid = atoi(argsv[1]);
                thisJob = get_job_from_jid(thisJid);

                if (thisJob->status == STOPPED || thisJob->status == BACKGROUND)
                {
                    thisJob->status = FOREGROUND;
                    print_cmdline(thisJob->pipe);
                    printf("\n");
                    termstate_give_terminal_to(&thisJob->saved_tty_state, thisJob->pid);
                    kill(thisJob->pid, SIGCONT);

                    signal_block(SIGCHLD);
                    wait_for_job(thisJob);
                    signal_unblock(SIGCHLD);
                    termstate_give_terminal_back_to_shell();                    
                }

                continue;
            }

            //add job
            struct job* currentJob = add_job(inputPipe);

            //set up file actions and clear all file descriptors
            posix_spawn_file_actions_init(&action);
            for (int i = 0; i <= maxfd; i++)
            {
                posix_spawn_file_actions_addclose(&action, i);
            }

            //redirect I/O
            posix_spawn_file_actions_addopen(&action, STDIN_FILENO, tty_name, O_RDWR, 0);
            posix_spawn_file_actions_adddup2(&action, STDIN_FILENO, STDOUT_FILENO);
            posix_spawn_file_actions_adddup2(&action, STDIN_FILENO, STDERR_FILENO);

            //set up file attr
            posix_spawnattr_t attr = {0};
            posix_spawnattr_init(&attr);

            //process group arrangement    
            if (!inputPipe->bg_job)
            {
                posix_spawnattr_setflags(&attr, POSIX_SPAWN_USEVFORK | POSIX_SPAWN_SETPGROUP | POSIX_SPAWN_TCSETPGROUP);
                posix_spawnattr_tcsetpgrp_np(&attr, termstate_get_tty_fd());   
            }
            else
                posix_spawnattr_setflags(&attr, POSIX_SPAWN_USEVFORK | POSIX_SPAWN_SETPGROUP);

            posix_spawnattr_setpgroup(&attr, 0);

            //Blocking SIGCHLD
            signal_block(SIGCHLD);

            //spawn child processes
            if ((status = posix_spawnp(&pid, argsv[0], &action, &attr, argsv, environ)) == 0)
            {
                //add process to num_processes
                currentJob->num_processes_alive++;
                termstate_save(&currentJob->saved_tty_state);

                //add the current pid to the currentJob
                currentJob->pid = pid;

                //waits for job to complete
                wait_for_job(currentJob);

                //unblock SIGCHILD
                signal_unblock(SIGCHLD);

                if (currentJob->status == BACKGROUND)
                {
                    printf("[%d] %d\n", currentJob->jid, currentJob->pid);
                }

                //returns the terminal back to the shell
                termstate_give_terminal_back_to_shell();
            } 
            else 
            {
                printf("%s\n", strerror(status));
            }

            //destroy child processes
            posix_spawn_file_actions_destroy(&action);
               
        }
        //delete jobs as they terminate
        for(int i = 1; i < MAXJOBS; ++i)
        {
            struct job* job = jid2job[i];
            if (job == NULL)
                break;

            if (job->num_processes_alive == 0)
            {
                delete_job(job);
            }
        }

        //free the command line
        list_init(&cline->pipes);
        ast_command_line_free(cline);
    }
    return 0;
}
