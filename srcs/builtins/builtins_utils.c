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

#include "../../minishell.h"
#include <string.h>

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
		return (ms_builtin_env(ctx));
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

/*
** ms_builtin_kind - komut adından builtin enum değeri döndür.
*/
t_bltin	ms_builtin_kind(const char *name)
{
	if (!name)
		return (BI_NONE);
	if (strcmp(name, "echo") == 0)
		return (BI_ECHO);
	if (strcmp(name, "pwd") == 0)
		return (BI_PWD);
	if (strcmp(name, "env") == 0)
		return (BI_ENV);
	if (strcmp(name, "export") == 0)
		return (BI_EXPORT);
	if (strcmp(name, "unset") == 0)
		return (BI_UNSET);
	if (strcmp(name, "cd") == 0)
		return (BI_CD);
	if (strcmp(name, "exit") == 0)
		return (BI_EXIT);
	return (BI_NONE);
}

/*
** ms_builtin_run - t_cmdnode arayüzü üzerinden builtin çalıştır.
*/
int	ms_builtin_run(t_ctx *ctx, t_cmdnode *cmd)
{
	if (!cmd || !cmd->argv || !cmd->argv[0])
		return (1);
	return (ms_builtin_run_argv(ctx, cmd->argv));
}
