/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   executer_pipeline.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hdere <hdere@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/08 12:00:00 by hdere             #+#    #+#             */
/*   Updated: 2026/03/26 20:42:23 by hdere            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "executor_internal.h"

void	child_setup_io(int *pipes, int n, int i)
{
	ms_sig_child_reset();
	if (i > 0)
		dup2(pipes[(i - 1) * 2], STDIN_FILENO);
	if (i < n - 1)
		dup2(pipes[i * 2 + 1], STDOUT_FILENO);
	ms_close_all_pipes(pipes, n - 1);
}

static int	open_pipes(int *pipes, int count)
{
	int	i;

	i = 0;
	while (i < count)
	{
		if (pipe(pipes + i * 2) < 0)
		{
			ms_close_all_pipes(pipes, i);
			return (0);
		}
		i++;
	}
	return (1);
}

static int	fork_handle_error(pid_t *pids, int done, int *pipes, int n)
{
	int	status;
	int	j;

	ms_close_all_pipes(pipes, n - 1);
	j = 0;
	while (j < done)
	{
		waitpid(pids[j], &status, 0);
		j++;
	}
	return (0);
}

static void	parent_close_used_ends(int *pipes, int n, int i)
{
	if (i < n - 1)
	{
		close(pipes[i * 2 + 1]);
		pipes[i * 2 + 1] = -1;
	}
	if (i > 0)
	{
		close(pipes[(i - 1) * 2]);
		pipes[(i - 1) * 2] = -1;
	}
}

int	ms_create_pipeline(t_ctx *ctx, t_cmdnode *pl, int *pipes, pid_t *pids)
{
	int	n;
	int	i;

	n = ms_count_cmds(pl);
	if (!open_pipes(pipes, n - 1))
		return (0);
	i = 0;
	while (i < n)
	{
		pids[i] = fork();
		if (pids[i] < 0)
			return (fork_handle_error(pids, i, pipes, n));
		if (pids[i] == 0)
		{
			child_setup_io(pipes, n, i);
			free(pipes);
			free(pids);
			child_run_cmd(ctx, pl);
		}
		parent_close_used_ends(pipes, n, i);
		pl = pl->next;
		i++;
	}
	return (1);
}
