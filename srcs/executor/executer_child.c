/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   executer_child.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hdere <hdere@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/08 12:00:00 by hdere             #+#    #+#             */
/*   Updated: 2026/03/08 07:37:59 by hdere            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "executor_internal.h"

void	close_other_heredoc_fds(t_ctx *ctx, t_cmdnode *my_cmd)
{
	t_cmdnode	*iter;
	t_heredoc	*hd;

	if (!ctx || !ctx->cur_ast)
		return ;
	iter = ctx->cur_ast;
	while (iter)
	{
		if (iter != my_cmd)
		{
			hd = iter->heredocs;
			while (hd)
			{
				if (hd->pipe_rd >= 0)
					close(hd->pipe_rd);
				hd->pipe_rd = -1;
				hd = hd->next;
			}
		}
		iter = iter->next;
	}
}

static void	run_builtin_or_exit(t_ctx *ctx, t_cmdnode *cmd)
{
	int	ret;
	int	status;

	if (ms_is_builtin_argv(cmd->argv))
	{
		ret = ms_builtin_run_argv(ctx, cmd->argv);
		if (ret == -1)
			status = ctx->last_status;
		else
			status = ret;
		child_exit(ctx, NULL, status);
	}
}

static void	free_ast_and_tokens(t_ctx *ctx, t_cmdnode *cmd)
{
	t_cmdnode	*node;
	t_cmdnode	*next;

	ms_token_free(ctx->cur_tokens);
	ctx->cur_tokens = NULL;
	node = ctx->cur_ast;
	while (node)
	{
		next = node->next;
		if (node != cmd)
			ms_cmd_free(node);
		node = next;
	}
	cmd->next = NULL;
	ctx->cur_ast = NULL;
}

static void	handle_cmd_not_found(t_ctx *ctx, char *cmd_name)
{
	ft_putstr_fd("minishell: ", STDERR_FILENO);
	ft_putstr_fd(cmd_name, STDERR_FILENO);
	if (errno == EISDIR)
		ft_putendl_fd(": Is a directory", STDERR_FILENO);
	else if (errno == EACCES)
		ft_putendl_fd(": Permission denied", STDERR_FILENO);
	else if (ft_strchr(cmd_name, '/'))
		ft_putendl_fd(": No such file or directory", STDERR_FILENO);
	else
		ft_putendl_fd(": command not found", STDERR_FILENO);
	if (errno == ENOENT)
		child_exit(ctx, NULL, 127);
	else
		child_exit(ctx, NULL, 126);
}

void	child_run_cmd(t_ctx *ctx, t_cmdnode *cmd)
{
	char	*path;

	if (!cmd->argv || !cmd->argv[0])
		child_exit(ctx, NULL, 0);
	close_other_heredoc_fds(ctx, cmd);
	if (!ms_apply_redirs(cmd, NULL, NULL))
		child_exit(ctx, NULL, 1);
	run_builtin_or_exit(ctx, cmd);
	if (ctx->env_dirty && !ms_env_build_envp(ctx))
		child_exit(ctx, NULL, 1);
	path = ms_resolve_path(ctx, cmd->argv[0]);
	if (!path)
		handle_cmd_not_found(ctx, cmd->argv[0]);
	free_ast_and_tokens(ctx, cmd);
	execve(path, cmd->argv, ctx->envp_cache);
	if (errno == ENOEXEC)
		exec_sh_fallback(cmd->argv, ctx->envp_cache);
	perror("minishell");
	ms_cmd_free(cmd);
	child_exit(ctx, path, 126);
}
