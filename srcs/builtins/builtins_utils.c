/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtins_utils.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hdere <hdere@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/08 12:00:00 by hdere             #+#    #+#             */
/*   Updated: 2026/03/08 07:08:40 by hdere            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	ms_is_builtin_argv(char **argv)
{
	if (!argv || !argv[0])
		return (0);
	if (strcmp(argv[0], "echo") == 0)
		return (1);
	if (strcmp(argv[0], "pwd") == 0)
		return (1);
	if (strcmp(argv[0], "env") == 0)
		return (1);
	if (strcmp(argv[0], "cd") == 0)
		return (1);
	if (strcmp(argv[0], "export") == 0)
		return (1);
	if (strcmp(argv[0], "unset") == 0)
		return (1);
	if (strcmp(argv[0], "exit") == 0)
		return (1);
	return (0);
}

int	ms_builtin_run_argv(t_ctx *ctx, char **argv)
{
	if (!argv || !argv[0])
		return (0);
	if (strcmp(argv[0], "echo") == 0)
		return (ms_builtin_echo(argv));
	if (strcmp(argv[0], "pwd") == 0)
		return (ms_builtin_pwd());
	if (strcmp(argv[0], "env") == 0)
		return (ms_builtin_env(ctx, argv));
	if (strcmp(argv[0], "cd") == 0)
		return (ms_builtin_cd(ctx, argv));
	if (strcmp(argv[0], "export") == 0)
		return (ms_builtin_export(ctx, argv));
	if (strcmp(argv[0], "unset") == 0)
		return (ms_builtin_unset(ctx, argv));
	if (strcmp(argv[0], "exit") == 0)
		return (ms_builtin_exit(ctx, argv));
	return (1);
}
