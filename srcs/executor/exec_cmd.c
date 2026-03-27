/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_cmd.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hdere <hdere@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/08 12:00:00 by hdere             #+#    #+#             */
/*   Updated: 2026/03/08 07:35:02 by hdere            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "executor_internal.h"

static void	exec_builtin_simple(t_ctx *ctx, t_cmdnode *cmd)
{
	int	saved_in;
	int	saved_out;
	int	ret;

	saved_in = -1;
	saved_out = -1;
	if ((cmd->redirs || cmd->heredocs)
		&& !ms_apply_redirs(cmd, &saved_in, &saved_out))
	{
		ctx->last_status = 1;
		return ;
	}
	if (cmd->argv && ft_strncmp(cmd->argv[0], "exit", 5) == 0
		&& ctx->interactive)
		ft_putendl_fd("exit", STDERR_FILENO);
	ret = ms_builtin_run_argv(ctx, cmd->argv);
	if (ret == -1)
	{
		ms_restore_stdio(saved_in, saved_out);
		ctx->exit_requested = true;
		return ;
	}
	ctx->last_status = ret;
	ms_restore_stdio(saved_in, saved_out);
}

static void	exec_empty_cmd(t_ctx *ctx, t_cmdnode *cmd)
{
	int	s_in;
	int	s_out;

	s_in = -1;
	s_out = -1;
	if (ms_apply_redirs(cmd, &s_in, &s_out))
		ctx->last_status = 0;
	else
		ctx->last_status = 1;
	ms_restore_stdio(s_in, s_out);
}

void	ms_exec_simple(t_ctx *ctx, t_cmdnode *cmd)
{
	char	*path;

	if (!cmd->argv || !cmd->argv[0])
	{
		exec_empty_cmd(ctx, cmd);
		return ;
	}
	if (ms_is_builtin_argv(cmd->argv))
	{
		exec_builtin_simple(ctx, cmd);
		return ;
	}
	if (ctx->env_dirty && !ms_env_build_envp(ctx))
	{
		ft_putendl_fd("minishell: fatal: env rebuild failed", STDERR_FILENO);
		ctx->last_status = 1;
		return ;
	}
	path = ms_resolve_path(ctx, cmd->argv[0]);
	if (!path)
		return (exec_cmd_not_found(ctx, cmd->argv[0]));
	fork_and_exec(ctx, cmd, path);
}
