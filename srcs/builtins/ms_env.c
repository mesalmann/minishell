/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ms_env.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hdere <hdere@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/08 12:00:00 by hdere             #+#    #+#             */
/*   Updated: 2026/03/08 07:08:53 by hdere            ###   ########.fr       */
/*                                                                        	  */
/* ************************************************************************** */

#include "minishell.h"

static void	env_child_proc(t_ctx *ctx, t_envnode *temp, char **argv, int idx)
{
	t_envnode	*curr;
	int			status;

	curr = temp;
	while (curr)
	{
		ms_env_set(ctx, curr->key, curr->val, true);
		curr = curr->next;
	}
	ms_builtin_run_argv(ctx, &argv[idx]);
	status = ctx->last_status;
	env_free_temp(temp);
	ms_ctx_destroy(ctx);
	_exit(status);
}

static int	env_wait_child(pid_t pid, t_ctx *ctx)
{
	int	status;

	while (waitpid(pid, &status, 0) == -1 && errno == EINTR)
		continue ;
	if (WIFEXITED(status))
		ctx->last_status = WEXITSTATUS(status);
	else if (WIFSIGNALED(status))
		ctx->last_status = 128 + WTERMSIG(status);
	return (ctx->last_status);
}

static int	env_execute_cmd(t_ctx *ctx, t_envnode *temp, char **argv, int idx)
{
	pid_t	pid;

	pid = fork();
	if (pid < 0)
	{
		perror("minishell: fork");
		return (1);
	}
	if (pid == 0)
		env_child_proc(ctx, temp, argv, idx);
	return (env_wait_child(pid, ctx));
}

int	ms_builtin_env(t_ctx *ctx, char **argv)
{
	t_envnode	*temp;
	int			cmd_idx;
	int			res;

	temp = NULL;
	res = parse_env_args(argv, &temp, &cmd_idx);
	if (res != 0)
	{
		env_free_temp(temp);
		return (res);
	}
	if (cmd_idx == -1)
	{
		env_print_with_overrides(ctx, temp);
		env_print_remain(ctx, temp);
		env_free_temp(temp);
		return (0);
	}
	res = env_execute_cmd(ctx, temp, argv, cmd_idx);
	env_free_temp(temp);
	return (res);
}
