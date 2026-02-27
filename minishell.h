/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: senadere <senadere@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/18 17:01:28 by mesalman          #+#    #+#             */
/*   Updated: 2026/02/26 23:49:57 by senadere         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MINISHELL_H
# define MINISHELL_H

/* ---- System headers ---- */
# include <stdbool.h>
# include <stddef.h>
# include <stdint.h>

/* UNIX */
# include <unistd.h>
# include <sys/types.h>
# include <sys/wait.h>
# include <sys/stat.h>
# include <fcntl.h>
# include <signal.h>
# include <errno.h>

/* libc */
# include <stdlib.h>
# include <stdio.h>
# include <string.h>

/* readline */
# include <readline/readline.h>
# include <readline/history.h>

/* ---- Subject rule: exactly one global for signals (only signal number) ---- */
extern volatile sig_atomic_t	g_sig;

/* ************************************************************************** */
/*                              CORE CONTRACT                                */
/* ************************************************************************** */
/*
** Parser guarantee:
**  - On syntax error: returns NULL and sets ctx->last_status = 2.
**  - On success: for each command node, argv[0] != NULL.
**
** Executor guarantee:
**  - If pipeline == NULL: do nothing.
**  - After each run: ctx->last_status updated (used by $? expansion).
*/

/* ************************************************************************** */
/*                                ENV MODEL                                  */
/* ************************************************************************** */

typedef struct s_envnode
{
	char				*key;      /* e.g. "PATH" */
	char				*val;      /* e.g. "/usr/bin:..." (may be NULL) */
	bool				has_val;   /* distinguishes KEY= vs KEY (export use-case) */
	struct s_envnode	*next;
}	t_envnode;

/* ************************************************************************** */
/*                              TOKEN / LEXER                                */
/* ************************************************************************** */

typedef enum e_tokkind
{
	TK_WORD = 1,   /* generic word (already de-quoted or still raw; your choice) */
	TK_OP,         /* one of: < > >> << | */
}	t_tokkind;

typedef enum e_opkind
{
	OP_NONE = 0,
	OP_IN,          /* <  */
	OP_OUT_TRUNC,   /* >  */
	OP_OUT_APPEND,  /* >> */
	OP_HEREDOC,     /* << */
	OP_PIPE         /* |  */
}	t_opkind;

typedef struct s_token
{
	t_tokkind	kind;
	t_opkind		op;      /* valid iff kind == TK_OP */
	char			*lex;    /* raw slice copy for WORD, or literal for OP */
	struct s_token	*next;
}	t_token;

/* ************************************************************************** */
/*                           REDIR / HEREDOC MODEL                            */
/* ************************************************************************** */

typedef enum e_redirtype
{
	RD_IN = 1,
	RD_OUT_TRUNC,
	RD_OUT_APPEND
}	t_redirtype;

typedef struct s_redir
{
	t_redirtype		type;
	char			*target;  /* filename AFTER expansion (or before, your policy) */
	struct s_redir	*next;
}	t_redir;

typedef struct s_heredoc
{
	char				*delim;        /* delimiter token text */
	bool				expand_mode;   /* true if delimiter NOT quoted */
	int					pipe_rd;       /* executor may set these */
	int					pipe_wr;
	struct s_heredoc	*next;
}	t_heredoc;

/* ************************************************************************** */
/*                              PARSER OUTPUT                                */
/* ************************************************************************** */

typedef struct s_cmdnode
{
	char				**argv;      /* NULL-terminated; argv[0] must exist */
	t_redir				*redirs;    /* <, >, >> list */
	t_heredoc			*heredocs;  /* << list (may be multiple) */
	struct s_cmdnode	*next;      /* pipeline chain */
}	t_cmdnode;

/* ************************************************************************** */
/*                                 CONTEXT                                   */
/* ************************************************************************** */

typedef struct s_ctx
{
	t_envnode	*env;          /* Bağlı liste (Linked list) başı */
	char		**envp_cache;  /* / execve'ye verilecek char** dizisi */
	bool		env_dirty;     // Cache yenilenmeli mi bayrağı (flag)

	int			last_status;   /* $? değeri (son komutun çıkış kodu)*/
	bool		interactive;   /* isatty(STDIN_FILENO)  Shell interaktif modda mı?*/
}	t_ctx;

/* ************************************************************************** */
/*                           MODULE API (PROTOTYPES)                          */
/* ************************************************************************** */

/* Utils (Libft fonksiyonlarının prototipleri) */
size_t  ft_strlen(const char *s);
char    *ft_strjoin(char const *s1, char const *s2);
char    *ft_strdup(const char *s1);
int     ft_strncmp(const char *s1, const char *s2, size_t n);
char    *ft_strchr(const char *s, int c);
void    *ft_memset(void *b, int c, size_t len);
char    **ft_split(char const *s, char c); // split.c ile uyumlu olmalı (tek char delimiter)
char    *ft_substr(char const *s, unsigned int start, size_t len);


/****************************Sena *************************/
void ms_loop(t_ctx *ctx, char **envp);
void    ms_exec_simple(t_ctx *ctx, char **argv, char **envp);
void    free_tab(char **tab);


/* init / teardown */
bool		ms_ctx_init(t_ctx *ctx, char **envp);
void		ms_ctx_destroy(t_ctx *ctx);

/* env */
char		*ms_env_get(t_ctx *ctx, const char *key);
bool		ms_env_set(t_ctx *ctx, const char *key, const char *val, bool has_val);
bool		ms_env_unset(t_ctx *ctx, const char *key);
char		**ms_env_build_envp(t_ctx *ctx); /* returns ctx->envp_cache */

/* signals */
void		ms_sig_install_interactive(void);

/* tokenizer */
t_token		*ms_tokenize(const char *line, t_ctx *ctx);
void		ms_token_free(t_token *toks);

/* expander */
char		*ms_expand_str(t_ctx *ctx, const char *in, bool in_dquote);
bool		ms_expand_argv(t_ctx *ctx, char ***argv_io);

/* parser */
t_cmdnode	*ms_parse(t_token *toks, t_ctx *ctx);
void		ms_cmd_free(t_cmdnode *cmd);

/* executor */
void		ms_execute_pipeline(t_ctx *ctx, t_cmdnode *pipeline);

/* path resolution */
char		*ms_resolve_path(t_ctx *ctx, const char *file);

/* redirections / heredoc */
bool		ms_apply_redirs(t_cmdnode *cmd, int *saved_stdin, int *saved_stdout);
void		ms_restore_stdio(int saved_stdin, int saved_stdout);
bool		ms_run_heredocs(t_ctx *ctx, t_cmdnode *pipeline);

/* builtins */
typedef enum e_bltin
{
	BI_NONE = 0,
	BI_ECHO,
	BI_PWD,
	BI_ENV,
	/* week2+ */
	BI_EXPORT,
	BI_UNSET,
	BI_CD,
	BI_EXIT
}	t_bltin;

t_bltin		ms_builtin_kind(const char *name);
int			ms_builtin_run(t_ctx *ctx, t_cmdnode *cmd);

/* errors */
int			ms_err_syntax(t_ctx *ctx, const char *msg); /* sets last_status=2 */
int			ms_err_errno(t_ctx *ctx, const char *where); /* uses errno */
int			count_words(char const *s, char c);

#endif