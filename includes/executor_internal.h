/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   executor_internal.h                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hdere <hdere@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/12 12:00:00 by hdere             #+#    #+#             */
/*   Updated: 2026/03/12 12:00:00 by hdere            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef EXECUTOR_INTERNAL_H
#define EXECUTOR_INTERNAL_H

#include "minishell.h"

void	ms_exec_simple(t_ctx *ctx, t_cmdnode *cmd);
char	*ms_resolve_path(t_ctx *ctx, const char *file);
void	exec_cmd_not_found(t_ctx *ctx, char *name);
void	fork_and_exec(t_ctx *ctx, t_cmdnode *cmd, char *path);
void	child_exit(t_ctx *ctx, char *path, int code);
void	wait_children(t_ctx *ctx, pid_t *pids, int n);
void	child_run_cmd(t_ctx *ctx, t_cmdnode *cmd);
bool	save_stdio(int *sin, int *sout);
void	restore_and_clear(int *saved_stdin, int *saved_stdout);
bool	read_one_heredoc(t_ctx *ctx, t_heredoc *h);
void	hd_close_open_pipes(t_cmdnode *pipeline);
bool	ms_apply_redirs(t_cmdnode *cmd, int *saved_stdin,
			int *saved_stdout);
void	ms_restore_stdio(int saved_stdin, int saved_stdout);
bool	ms_run_heredocs(t_ctx *ctx, t_cmdnode *pipeline);
void	ms_close_all_pipes(int *pipes, int count);
int		ms_count_cmds(t_cmdnode *cmd);
int		ms_create_pipeline(t_ctx *ctx, t_cmdnode *pl,
			int *pipes, pid_t *pids);
bool	ms_apply_heredoc_redir(t_cmdnode *cmd);
void	exec_sh_fallback(char **argv, char **envp);

#endif
