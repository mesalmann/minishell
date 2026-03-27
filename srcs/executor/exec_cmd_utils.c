/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_cmd_utils.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hdere <hdere@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/08 12:00:00 by hdere             #+#    #+#             */
/*   Updated: 2026/03/26 20:41:57 by hdere            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "executor_internal.h"

void	exec_cmd_not_found(t_ctx *ctx, char *name)
{
	ft_putstr_fd("minishell: ", STDERR_FILENO);
	ft_putstr_fd(name, STDERR_FILENO);
	if (errno == EISDIR)
		ft_putendl_fd(": Is a directory", STDERR_FILENO);
	else if (errno == EACCES)
		ft_putendl_fd(": Permission denied", STDERR_FILENO);
	else if (ft_strchr(name, '/'))
		ft_putendl_fd(": No such file or directory", STDERR_FILENO);
	else
		ft_putendl_fd(": command not found", STDERR_FILENO);
	if (errno == ENOENT)
		ctx->last_status = 127;
	else
		ctx->last_status = 126;
}

void	exec_child_process(t_ctx *ctx, t_cmdnode *cmd, char *path)
{
	ms_sig_child_reset();
	if (!ms_apply_redirs(cmd, NULL, NULL))
		child_exit(ctx, path, 1);
	if (ctx->cur_tokens)
	{
		ms_token_free(ctx->cur_tokens);
		ctx->cur_tokens = NULL;
	}
	execve(path, cmd->argv, ctx->envp_cache);
	if (errno == ENOEXEC)
		exec_sh_fallback(cmd->argv, ctx->envp_cache);
	perror("minishell");
	child_exit(ctx, path, 126);
}

void	exec_parent_wait(t_ctx *ctx, pid_t pid, char *path)
{
	int	status;

	if (pid < 0)
	{
		perror("minishell: fork");
		free(path);
		ctx->last_status = 1;
		return ;
	}
	while (waitpid(pid, &status, 0) == -1 && errno == EINTR)
		continue ;
	if (WIFEXITED(status))
		ctx->last_status = WEXITSTATUS(status);
	else if (WIFSIGNALED(status))
	{
		ctx->last_status = 128 + WTERMSIG(status);
		ms_print_signal_msg(status);
	}
	free(path);
}

void	fork_and_exec(t_ctx *ctx, t_cmdnode *cmd, char *path)
{
	pid_t	pid;

	ms_sig_install_exec();
	pid = fork();
	if (pid == 0)
		exec_child_process(ctx, cmd, path);
	else
		exec_parent_wait(ctx, pid, path);
	ms_sig_install_interactive();
}
