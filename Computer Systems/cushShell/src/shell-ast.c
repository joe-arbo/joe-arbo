/*
 * shell-ast
 * Manage the abstract syntax tree resulting from parsing
 * command lines from a simple shell grammar.
 *
 * Refactored by Godmar Back for CS 3214 Summer 2020
 * Virginia Tech.
 */
#include <stdio.h>
#include <sys/types.h>
#include <limits.h>
#include <stdlib.h>

#include "shell-ast.h"

/* Create new command structure.  Takes ownership of argv. */
struct ast_command * 
ast_command_create(char ** argv, bool dup_stderr_to_stdout)
{
    struct ast_command *cmd = malloc(sizeof *cmd);

    cmd->argv = argv;
    cmd->dup_stderr_to_stdout = dup_stderr_to_stdout;
    return cmd;
}

/* Create a new pipeline */
struct ast_pipeline * ast_pipeline_create(char *iored_input, 
                                          char *iored_output, 
                                          bool append_to_output)
{
    struct ast_pipeline *pipe = malloc(sizeof *pipe);

    list_init(&pipe->commands);
    pipe->iored_output = iored_output;
    pipe->iored_input = iored_input;
    pipe->append_to_output = append_to_output;
    pipe->bg_job = false;
    return pipe;
}

/* Add a new command to this pipeline */
void
ast_pipeline_add_command(struct ast_pipeline *pipe, struct ast_command *cmd)
{
    list_push_back(&pipe->commands, &cmd->elem);
}

/* Create an empty command line */
struct ast_command_line *
ast_command_line_create_empty(void)
{
    struct ast_command_line *cmdline = malloc(sizeof *cmdline);

    list_init(&cmdline->pipes);
    return cmdline;
}

/* Create a command line with a single pipeline */
struct ast_command_line *
ast_command_line_create(struct ast_pipeline *pipe)
{
    struct ast_command_line *cmdline = ast_command_line_create_empty();

    list_push_back(&cmdline->pipes, &pipe->elem);
    return cmdline;
}

/* Print ast_command structure to stdout */
void
ast_command_print(struct ast_command *cmd)
{
    char **p = cmd->argv;

    printf("  Command:");
    while (*p)
        printf(" %s", *p++);

    printf("\n");

    if (cmd->dup_stderr_to_stdout)
        printf("  stderr shall also be redirected\n");
}
  
/* Print ast_pipeline structure to stdout */
void
ast_pipeline_print(struct ast_pipeline *pipe)
{
    int i = 1;

    printf(" Pipeline consists of %ld commands\n", list_size(&pipe->commands));
    for (struct list_elem * e = list_begin(&pipe->commands); 
         e != list_end(&pipe->commands); 
         e = list_next(e)) {
        struct ast_command *cmd = list_entry(e, struct ast_command, elem);

        printf(" %d. ", i++);
        ast_command_print(cmd);
    }

    if (pipe->iored_output)
        printf("  the stdout of the last command %ss to %s\n", 
                pipe->append_to_output ? "append" : "write",
                pipe->iored_output);

    if (pipe->iored_input)
        printf("  stdin of the first command reads from %s\n", pipe->iored_input);

    if (pipe->bg_job)
        printf("  - is a background job\n");
    else
        printf("  - should be started as a regular foreground job\n");
}

/* Print ast_command_line structure to stdout */
void 
ast_command_line_print(struct ast_command_line *cmdline)
{
    printf("Command line\n");
    for (struct list_elem * e = list_begin (&cmdline->pipes); 
         e != list_end (&cmdline->pipes); 
         e = list_next (e)) {
        struct ast_pipeline *pipe = list_entry(e, struct ast_pipeline, elem);

        printf(" ------------- \n");
        ast_pipeline_print(pipe);
    }
    printf("==========================================\n");
}

/* Deallocation functions. */
void 
ast_command_line_free(struct ast_command_line *cmdline)
{
    for (struct list_elem * e = list_begin(&cmdline->pipes); e != list_end(&cmdline->pipes); ) {
        struct ast_pipeline *pipe = list_entry(e, struct ast_pipeline, elem);
        e = list_remove(e);
        ast_pipeline_free(pipe);
    }
    free(cmdline);
}

void 
ast_pipeline_free(struct ast_pipeline *pipe)
{
    for (struct list_elem * e = list_begin(&pipe->commands); e != list_end(&pipe->commands); ) {
        struct ast_command *cmd = list_entry(e, struct ast_command, elem);
        e = list_remove(e);
        ast_command_free(cmd);
    }
    free(pipe);
}

void 
ast_command_free(struct ast_command * cmd)
{
    char ** p = cmd->argv;
    while (*p) {
        free(*p++);
    }
    free(cmd->argv);
    free(cmd);
}
