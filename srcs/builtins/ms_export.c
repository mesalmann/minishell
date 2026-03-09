/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ms_export.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hdere <hdere@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/08 12:00:00 by hdere             #+#    #+#             */
/*   Updated: 2026/03/08 07:33:01 by hdere            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../minishell.h"

static int export_invalid_id(const char *arg)
{
	ft_putstr_fd("minishell: export: `", STDERR_FILENO);
	ft_putstr_fd(arg, STDERR_FILENO);
	ft_putendl_fd("': not a valid identifier", STDERR_FILENO);
	return (1);
}

static int export_set_one(t_ctx *ctx, char *arg)
{
	char *eq;
	char *key;

	eq = ft_strchr(arg, '=');
	if (eq)
	{
		key = ft_substr(arg, 0, (unsigned int)(eq - arg));
		if (!key)
			return (-1);
		if (!ms_is_valid_identifier(key))
		{
			free(key);
			return (export_invalid_id(arg));
		}
		ms_env_set(ctx, key, eq + 1, true);
		free(key);
		return (0);
	}
	if (!ms_is_valid_identifier(arg))
		return (export_invalid_id(arg));
	ms_env_set(ctx, arg, NULL, false);
	return (0);
}

int ms_builtin_export(t_ctx *ctx, char **argv)
{
	int i;
	int ret;
	int tmp;

	if (!argv[1])
	{
		print_export(ctx);
		return (0);
	}
	ret = 0;
	i = 1;
	while (argv[i])
	{
		tmp = export_set_one(ctx, argv[i]);
		if (tmp < 0)
			return (1);
		if (tmp > 0)
			ret = tmp;
		i++;
	}
	return (ret);
}
