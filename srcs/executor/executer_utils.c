/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   executer_utils.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hdere <hdere@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/08 12:00:00 by hdere             #+#    #+#             */
/*   Updated: 2026/03/26 20:37:43 by hdere            ###   ########.fr       */
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
		if (pipes[i * 2] >= 0)
			close(pipes[i * 2]);
		if (pipes[i * 2 + 1] >= 0)
			close(pipes[i * 2 + 1]);
		i++;
	}
}

void	wait_children(t_ctx *ctx, pid_t *pids, int n)
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

void	child_exit(t_ctx *ctx, char *path, int code)
{
	if (ctx)
	{
		if (ctx->cur_tokens)
		{
			ms_token_free(ctx->cur_tokens);
			ctx->cur_tokens = NULL;
		}
		if (ctx->cur_ast)
		{
			ms_cmd_free_list(ctx->cur_ast);
			ctx->cur_ast = NULL;
		}
	}
	ms_ctx_destroy(ctx);
	free(path);
	_exit(code);
}

void	exec_sh_fallback(char **argv, char **envp)
{
	char	**nargv;
	int		argc;
	int		i;

	argc = 0;
	while (argv[argc])
		argc++;
	nargv = malloc(sizeof(char *) * (argc + 2));
	if (!nargv)
		return ;
	nargv[0] = "/bin/sh";
	i = 0;
	while (i <= argc)
	{
		nargv[i + 1] = argv[i];
		i++;
	}
	execve("/bin/sh", nargv, envp);
	if (errno != 0)
		free(nargv);
}
