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

#include "minishell.h"

static int export_invalid_id(const char *arg)
{
	ft_putstr_fd("minishell: export: '", STDERR_FILENO);
	ft_putstr_fd(arg, STDERR_FILENO);
	ft_putendl_fd("': not a valid identifier", STDERR_FILENO);
	return (1);
}

static int	export_do_assign(t_ctx *ctx, char *key, char *val, int app)
{
	char	*old;
	char	*joined;

	if (app)
	{
		old = ms_env_get(ctx, key);
		if (old)
		{
			joined = ft_strjoin(old, val);
			if (!joined)
			{
				free(key);
				return (-1);
			}
			ms_env_set(ctx, key, joined, true);
			free(joined);
			free(key);
			return (0);
		}
	}
	ms_env_set(ctx, key, val, true);
	free(key);
	return (0);
}

static int	export_set_one(t_ctx *ctx, char *arg)
{
	char	*eq;
	char	*key;
	int		append;

	eq = ft_strchr(arg, '=');
	if (!eq)
	{
		if (!ms_is_valid_identifier(arg))
			return (export_invalid_id(arg));
		ms_env_set(ctx, arg, NULL, false);
		return (0);
	}
	append = (eq > arg && *(eq - 1) == '+');
	if (append)
		key = ft_substr(arg, 0, (unsigned int)(eq - arg - 1));
	else
		key = ft_substr(arg, 0, (unsigned int)(eq - arg));
	if (!key)
		return (-1);
	if (!ms_is_valid_identifier(key))
	{
		free(key);
		return (export_invalid_id(arg));
	}
	return (export_do_assign(ctx, key, eq + 1, append));
}

static int	export_invalid_opt(const char *arg)
{
	ft_putstr_fd("minishell: export: -", STDERR_FILENO);
	write(STDERR_FILENO, &arg[1], 1);
	ft_putendl_fd(": invalid option", STDERR_FILENO);
	return (2);
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
		if (argv[i][0] == '-' && argv[i][1])
			return (export_invalid_opt(argv[i]));
		tmp = export_set_one(ctx, argv[i]);
		if (tmp < 0)
			return (1);
		if (tmp > 0)
			ret = tmp;
		i++;
	}
	return (ret);
}
