#ifndef __SHELL_AST_H
#define __SHELL_AST_H

#include "list.h"

/* Forward declarations. */
struct ast_command;
struct ast_pipeline;
struct ast_command_line;

/* A command line may contain multiple pipelines. */
struct ast_command_line {
    struct list/* <ast_pipeline> */ pipes;        /* List of pipelines */
};

/* A pipeline is a list of one or more commands. 
 * For the purposes of job control, a pipeline forms one job.
 */
struct ast_pipeline {
    struct list/* <ast_command> */ commands;    /* List of commands */
    char *iored_input;       /* If non-NULL, first command should read from
                                file 'iored_input' */
    char *iored_output;      /* If non-NULL, last command should write to
                                file 'iored_output' */
    bool append_to_output;   /* True if user typed >> to append */
    bool bg_job;             /* True if user entered & */
    struct list_elem elem;   /* Link element. */
};

/* A command is part of a pipeline. */
struct ast_command {
    char **argv;             /* NULL terminated array of pointers to words
                                making up this command. */
    bool dup_stderr_to_stdout; /* True if stderr should be redirected as well */
    struct list_elem elem;   /* Link element to link commands in pipeline. */
};

/* Create new command structure and initialize it */
struct ast_command * ast_command_create(char ** argv,
                                        bool dup_stderr_to_stdout);

/* Create a new pipeline containing only one command */
struct ast_pipeline * ast_pipeline_create(char *iored_input, 
                                          char *iored_output, 
                                          bool append_to_output);

/* Add a new command to this pipeline */
void ast_pipeline_add_command(struct ast_pipeline *pipe, struct ast_command *cmd);

/* Create an empty command line */
struct ast_command_line * ast_command_line_create_empty(void);

/* Create a command line with a single pipeline */
struct ast_command_line * ast_command_line_create(struct ast_pipeline *pipe);

/* Deallocation functions */
void ast_command_line_free(struct ast_command_line *);
void ast_pipeline_free(struct ast_pipeline *);
void ast_command_free(struct ast_command *);

/* Print functions */
void ast_command_print(struct ast_command *cmd);
void ast_pipeline_print(struct ast_pipeline *pipe);
void ast_command_line_print(struct ast_command_line *line);

/* Parse a command line.  Implemented in shell-grammar.y */
struct ast_command_line * ast_parse_command_line(char * line);

/** ----------------------------------------------------------- */
#endif /* __SHELL_AST_H */
