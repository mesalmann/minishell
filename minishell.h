/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hdere <hdere@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/18 17:01:28 by mesalman          #+#    #+#             */
/*   Updated: 2026/03/08 07:50:00 by hdere            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MINISHELL_H
#define MINISHELL_H

#include <ctype.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <readline/history.h>
#include <readline/readline.h>

extern volatile sig_atomic_t g_sig;

typedef struct s_envnode
{
	char *key;
	char *val;
	bool has_val;
	struct s_envnode *next;
} t_envnode;

typedef enum e_tokkind
{
	TK_WORD = 1,
	TK_OP,
} t_tokkind;

typedef enum e_qstate
{
	Q_NONE = 0,
	Q_SINGLE,
	Q_DOUBLE
} t_qstate;

typedef enum e_opkind
{
	OP_NONE = 0,
	OP_IN,
	OP_OUT_TRUNC,
	OP_OUT_APPEND,
	OP_HEREDOC,
	OP_PIPE
} t_opkind;

typedef struct s_token
{
	t_tokkind kind;
	t_opkind op;
	char *lex;
	bool no_expand;
	struct s_token *next;
} t_token;

typedef enum e_redirtype
{
	RD_IN = 1,
	RD_OUT_TRUNC,
	RD_OUT_APPEND
} t_redirtype;

typedef struct s_redir
{
	t_redirtype type;
	char *target;
	struct s_redir *next;
} t_redir;

typedef struct s_heredoc
{
	char *delim;
	bool expand_mode;
	int pipe_rd;
	int pipe_wr;
	struct s_heredoc *next;
} t_heredoc;

typedef struct s_cmdnode
{
	char **argv;
	t_redir *redirs;
	t_heredoc *heredocs;
	struct s_cmdnode *next;
} t_cmdnode;

typedef struct s_ctx
{
	t_envnode *env;
	char **envp_cache;
	bool env_dirty;
	int last_status;
	bool interactive;
} t_ctx;

typedef struct s_expbuf
{
	char *out;
	size_t len;
	size_t cap;
	int i;
	t_qstate st;
} t_expbuf;

typedef enum e_bltin
{
	BI_NONE = 0,
	BI_ECHO,
	BI_PWD,
	BI_ENV,
	BI_EXPORT,
	BI_UNSET,
	BI_CD,
	BI_EXIT
} t_bltin;

size_t ft_strlen(const char *s);
char *ft_strjoin(char const *s1, char const *s2);
char *ft_strdup(const char *s1);
int ft_strncmp(const char *s1, const char *s2, size_t n);
char *ft_strchr(const char *s, int c);
void *ft_memset(void *b, int c, size_t len);
char **ft_split(char const *s, char c);
char *ft_substr(char const *s, unsigned int start, size_t len);
void ft_putstr_fd(const char *s, int fd);
void ft_putendl_fd(const char *s, int fd);
char *ft_itoa(int n);
void ms_loop(t_ctx *ctx, char **envp);
void ms_exec_simple(t_ctx *ctx, t_cmdnode *cmd);
void free_tab(char **tab);
bool ms_ctx_init(t_ctx *ctx, char **envp);
void ms_ctx_destroy(t_ctx *ctx);
int ms_is_builtin_argv(char **argv);
int ms_builtin_run_argv(t_ctx *ctx, char **argv);
int ms_builtin_pwd(void);
int ms_builtin_echo(char **argv);
int	ms_builtin_env(t_ctx *ctx, char **argv);
int ms_builtin_cd(t_ctx *ctx, char **argv);
int ms_builtin_export(t_ctx *ctx, char **argv);
int ms_builtin_unset(t_ctx *ctx, char **argv);
int ms_builtin_exit(t_ctx *ctx, char **argv);
char *ms_env_get(t_ctx *ctx, const char *key);
bool ms_env_set(t_ctx *ctx, const char *key, const char *val,
				bool has_val);
bool ms_env_unset(t_ctx *ctx, const char *key);
char **ms_env_build_envp(t_ctx *ctx);
void ms_sig_install_interactive(void);
t_token *ms_tokenize(const char *line, t_ctx *ctx);
void ms_token_free(t_token *toks);
bool ms_syntax_validate(t_token *tokens, t_ctx *ctx);
bool ms_expand_tokens(t_token **tokens, t_ctx *ctx);
char *ms_expand_str(t_ctx *ctx, const char *in, bool in_dquote);
bool ms_expand_argv(t_ctx *ctx, char ***argv_io);
t_cmdnode *ms_parse(t_token *toks, t_ctx *ctx);
void ms_cmd_free(t_cmdnode *cmd);
void ms_execute_pipeline(t_ctx *ctx, t_cmdnode *pipeline);
char *ms_resolve_path(t_ctx *ctx, const char *file);
bool ms_apply_redirs(t_cmdnode *cmd, int *saved_stdin,
					 int *saved_stdout);
void ms_restore_stdio(int saved_stdin, int saved_stdout);
bool ms_run_heredocs(t_ctx *ctx, t_cmdnode *pipeline);
t_bltin ms_builtin_kind(const char *name);
int ms_builtin_run(t_ctx *ctx, t_cmdnode *cmd);
int ms_err_syntax(t_ctx *ctx, const char *msg);
int ms_err_errno(t_ctx *ctx, const char *where);
int count_words(char const *s, char c);
t_token *token_new(t_tokkind kind, t_opkind op, char *lex);
int ms_is_valid_identifier(const char *s);
void print_export(t_ctx *ctx);
void ms_close_all_pipes(int *pipes, int count);
int ms_count_cmds(t_cmdnode *cmd);
int ms_create_pipeline(t_ctx *ctx, t_cmdnode *pl,
					   int *pipes, pid_t *pids);
bool ms_apply_heredoc_redir(t_cmdnode *cmd);
bool ms_sb_grow(char **buf, size_t *cap, size_t need);
bool ms_sb_pushc(char **buf, size_t *len, size_t *cap, char c);
bool ms_sb_pushs(char **buf, size_t *len, size_t *cap, const char *s);
bool ms_is_var_start(char c);
bool ms_is_var_char(char c);
t_cmdnode *ms_cmd_new(void);
t_redir *ms_redir_new(t_redirtype type, char *target);
t_heredoc *ms_heredoc_new(char *delim, bool expand_mode);
void ms_redir_add_back(t_redir **lst, t_redir *new);
void ms_heredoc_add_back(t_heredoc **lst, t_heredoc *new);
bool ms_fill_argv(t_cmdnode *cmd, t_token *t);
bool ms_parse_ios(t_cmdnode *cmd, t_token *t);
bool ms_is_redir_or_heredoc(t_opkind op);
void ms_cmd_free_list(t_cmdnode *head);
#endif
