/*
 * Grammar for csh-like shell
 *
 * Updated Summer 2020
 *
 * Developed by Godmar Back for CS 3214 Fall 2009
 * Virginia Tech.
 *
 * This is based on an assignment as an undergraduate in 1993 
 * as an undergraduate student at Technische Universitaet Berlin.
 *
 * Known bugs: leaks memory when parse errors occur.
 */
%{
#include <stdio.h>
#include <stdlib.h>
#define YYDEBUG	1
int yydebug;
void yyerror(const char *msg);
int yylex(void);

/*
 * Error messages, csh-style
 */
#define MISRED	"Missing name for redirect."
#define INVNUL  "Invalid null command."
#define AMBINP  "Ambiguous input redirect."
#define AMBOUT  "Ambiguous output redirect."

#include "shell-ast.h"
#include <obstack.h>
#include <assert.h>

#define obstack_chunk_alloc malloc
#define obstack_chunk_free free

struct cmd_helper {
    struct obstack words;   /* an obstack of char * to collect argv */
    char *iored_input;
    char *iored_output;
    bool append_to_output;
    bool redirect_stderr;
    struct list_elem elem;
};

struct pipe_helper {
    struct list commands;
};

static struct pipe_helper *
init_pipe()
{
    struct pipe_helper * pipe = calloc(1, sizeof *pipe);
    list_init(&pipe->commands);
    return pipe;
}

/* Initialize cmd_helper and, optionally, set first argv */
static struct cmd_helper *
init_cmd(char *firstcmd, 
         char *iored_input, char *iored_output, 
         bool append_to_output, bool include_stderr)
{
    struct cmd_helper * cmd = malloc(sizeof *cmd);
    obstack_init(&cmd->words);
    if (firstcmd)
        obstack_ptr_grow(&cmd->words, firstcmd);

    cmd->iored_output = iored_output;
    cmd->iored_input = iored_input;
    cmd->append_to_output = append_to_output;
    cmd->redirect_stderr = include_stderr;
    return cmd;
}

/* print error message */
static void p_error(char *msg);

/* Convert cmd_helper to ast_command.
 * Ensures NULL-terminated argv[] array
 */
static struct ast_command * 
make_ast_command(struct cmd_helper *cmd)
{
    obstack_ptr_grow(&cmd->words, NULL);

    int sz = obstack_object_size(&cmd->words);
    char **argv = malloc(sz);
    memcpy(argv, obstack_finish(&cmd->words), sz);
    obstack_free(&cmd->words, NULL);

    if (*argv == NULL) {
        free(argv);
        return NULL; 
    }

    return ast_command_create(argv, cmd->redirect_stderr);
}

static bool
add_to_pipeline(struct pipe_helper *pipe,
                struct cmd_helper *cmd,
                bool redirect_stderr)
{
    if (!list_empty(&pipe->commands)) {
        struct cmd_helper * last;
        last = list_entry(list_back(&pipe->commands), 
                          struct cmd_helper, elem);
        /* Error: 'ls >x | wc' */
        if (last->iored_output) { p_error(AMBOUT); return false; }
        last->redirect_stderr = redirect_stderr;

        /* Error: 'ls | <x wc' */
        if (cmd->iored_input) { p_error(AMBINP); return false; }
    }

    int sz = obstack_object_size(&cmd->words);
    if (sz == 0) { p_error(INVNUL); return false; }

    list_push_back(&pipe->commands, &cmd->elem);
    return true;
}

/* Called by parser when command line is complete */
static void cmdline_complete(struct ast_command_line *);

/* work-around for bug in flex 2.31 and later */
static void yyunput (int c,char *buf_ptr  ) __attribute__((unused));

%}

/* LALR stack types */
%union {
  struct cmd_helper *command;
  struct pipe_helper *pipe;
  struct ast_pipeline *ast_pipe;
  struct ast_command_line *cmdline;
  char *word;
}

/* Nonterminals */
%type <command> input output
%type <command> command
%type <pipe> pipeline
%type <ast_pipe> ast_pipeline
%type <cmdline> cmd_list

/* Terminals */
%token <word> WORD
%token GREATER_GREATER GREATER_AMPERSAND PIPE_AMPERSAND

%%
cmd_line: cmd_list { cmdline_complete($1); }

cmd_list:	/* Null Command */ { $$ = ast_command_line_create_empty(); }
|		ast_pipeline { 
            $$ = ast_command_line_create($1);
        } 
|		cmd_list ';'
|		cmd_list '&' {
            $$ = $1;
            struct ast_pipeline * last;
            last = list_entry(list_back(&$1->pipes), 
                              struct ast_pipeline, elem);
            last->bg_job = true;
        }
|		cmd_list ';' ast_pipeline	{ 
            $$ = $1;
            list_push_back(&$$->pipes, &$3->elem);
        }
|		cmd_list '&' ast_pipeline	{ 
            struct ast_pipeline * last;
            last = list_entry(list_back(&$1->pipes), 
                              struct ast_pipeline, elem);
            last->bg_job = true;

            $$ = $1;
            list_push_back(&$$->pipes, &$3->elem);
        }

ast_pipeline: pipeline {
            struct pipe_helper * pipe = $1;
            assert (!list_empty(&pipe->commands));
            struct cmd_helper * first;
            first = list_entry(list_front(&pipe->commands), struct cmd_helper, elem);
            struct cmd_helper * last;
            last = list_entry(list_back(&pipe->commands), struct cmd_helper, elem);

            $$ = ast_pipeline_create(
                first->iored_input,
                last->iored_output,
                last->append_to_output
            );
            for (struct list_elem * e = list_begin(&pipe->commands);
                                    e != list_end(&pipe->commands);) {
                struct cmd_helper * cmd = list_entry(e, struct cmd_helper, elem);
                ast_pipeline_add_command($$, make_ast_command(cmd));
                e = list_remove(e);
                free(cmd);
            }
            free(pipe);
        }

pipeline: command {
            $$ = init_pipe();
            if (!add_to_pipeline($$, $1, false))
                YYABORT;
		}
|		pipeline '|' command {
            if (!add_to_pipeline($1, $3, false))
                YYABORT;
            $$ = $1;
		}
|		pipeline PIPE_AMPERSAND command {
            if (!add_to_pipeline($1, $3, true))
                YYABORT;
            $$ = $1;
		}
|		'|' error 	   { p_error(INVNUL); YYABORT; }
|		pipeline '|' error { p_error(INVNUL); YYABORT; }

command:   WORD { 
            $$ = init_cmd($1, NULL, NULL, false, false);
        }
|		input   
|		output
|		command WORD {
            $$ = $1;
            obstack_ptr_grow(&$$->words, $2);
		}
|		command input {
            obstack_free(&$2->words, NULL);
            /* Error: ambiguous redirect 'a <b <c' */
            if ($1->iored_input)   { p_error(AMBINP); YYABORT; }
            $$ = $1; 
            $$->iored_input = $2->iored_input;
            free($2);
		}
|		command output {
            obstack_free(&$2->words, NULL);
            /* Error: ambiguous redirect 'a >b >c' */
            if ($1->iored_output) { p_error(AMBOUT); YYABORT; }
            $$ = $1; 
            $$->iored_output = $2->iored_output;
            $$->append_to_output = $2->append_to_output;
            $$->redirect_stderr = $2->redirect_stderr;
            free($2);
		}

input:	'<' WORD { 
            $$ = init_cmd(NULL, $2, NULL, false, false);
        }
|		'<' error	  { p_error(MISRED); YYABORT; }

output:	'>' WORD { 
            $$ = init_cmd(NULL, NULL, $2, false, false);
        }
|		GREATER_AMPERSAND WORD { 
            $$ = init_cmd(NULL, NULL, $2, false, true);
        }
|		GREATER_GREATER WORD { 
            $$ = init_cmd(NULL, NULL, $2, true, false);
        }
		/* Error: missing redirect */
|		'>' error 	  { p_error(MISRED); YYABORT; }
|		GREATER_GREATER error { p_error(MISRED); YYABORT; }

%%
static char * inputline;    /* currently processed input line */
#define YY_INPUT(buf,result,max_size) \
    { \
        result = *inputline ? (buf[0] = *inputline++, 1) : YY_NULL; \
    }

#define YY_NO_INPUT
#include "lex.yy.c"

static void
p_error(char *msg) 
{ 
    /* print error */
    fprintf(stderr, "%s\n", msg); 
}

extern int yyparse (void);

/* do not use default error handling since errors are handled above. */
void 
yyerror(const char *msg) { }

static struct ast_command_line * commandline;
static void cmdline_complete(struct ast_command_line *cline)
{
    commandline = cline;
}

/* 
 * parse a commandline.
 */
struct ast_command_line *
ast_parse_command_line(char * line)
{
    inputline = line;
    commandline = NULL;

    int error = yyparse();

    return error ? NULL : commandline;
}
