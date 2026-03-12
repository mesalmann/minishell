/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   executer.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hdere <hdere@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/08 12:00:00 by hdere             #+#    #+#             */
/*   Updated: 2026/03/08 07:54:03 by hdere            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "executor_internal.h"

int	ms_count_cmds(t_cmdnode *cmd)
{
	int	n;

	n = 0;
	while (cmd)
	{
		n++;
		cmd = cmd->next;
	}
	return (n);
}

void	ms_close_all_pipes(int *pipes, int count)
{
	int	i;

	i = 0;
	while (i < count)
	{
		close(pipes[i * 2]);
		close(pipes[i * 2 + 1]);
		i++;
	}
}

static void	wait_children(t_ctx *ctx, pid_t *pids, int n)
{
	int	i;
	int	status;

	i = 0;
	while (i < n)
	{
		while (waitpid(pids[i], &status, 0) == -1 && errno == EINTR)
			continue ;
		if (i == n - 1)
		{
			if (WIFEXITED(status))
				ctx->last_status = WEXITSTATUS(status);
			else if (WIFSIGNALED(status))
			{
				ctx->last_status = 128 + WTERMSIG(status);
				ms_print_signal_msg(status);
			}
		}
		i++;
	}
}

static void ms_exec_pipeline_multi(t_ctx *ctx, t_cmdnode *pipeline)
{
	int n;
	int *pipes;
	pid_t *pids;

	n = ms_count_cmds(pipeline);
	pipes = malloc(sizeof(int) * 2 * (n - 1));
	pids = malloc(sizeof(pid_t) * n);
	if (!pipes || !pids)
	{
		free(pipes);
		free(pids);
		ctx->last_status = 1;
		return;
	}
	ms_sig_install_exec();
	if (!ms_create_pipeline(ctx, pipeline, pipes, pids))
	{
		ms_sig_install_interactive();
		free(pipes);
		free(pids);
		return;
	}
	wait_children(ctx, pids, n);
	ms_sig_install_interactive();
	free(pipes);
	free(pids);
}

static void	ms_update_underscore(t_ctx *ctx, t_cmdnode *pipeline)
{
	t_cmdnode	*last;
	int			i;

	last = pipeline;
	while (last->next)
		last = last->next;
	if (!last->argv || !last->argv[0])
		return ;
	i = 0;
	while (last->argv[i + 1])
		i++;
	ms_env_set(ctx, "_", last->argv[i], true);
}

void ms_execute_pipeline(t_ctx *ctx, t_cmdnode *pipeline)
{
	if (!pipeline)
		return;
	if (!ms_run_heredocs(ctx, pipeline))
	{
		ctx->last_status = 130;
		return;
	}
	if (!pipeline->next)
	{
		ms_exec_simple(ctx, pipeline);
		ms_update_underscore(ctx, pipeline);
		return;
	}
	ms_exec_pipeline_multi(ctx, pipeline);
	ms_update_underscore(ctx, pipeline);
}
