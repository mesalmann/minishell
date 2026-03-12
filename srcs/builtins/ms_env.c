/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ms_env.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hdere <hdere@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/08 12:00:00 by hdere             #+#    #+#             */
/*   Updated: 2026/03/08 07:08:53 by hdere            ###   ########.fr       */
/*                                                                        	    */
/* ************************************************************************** */

#include "minishell.h"

int ms_builtin_env(t_ctx *ctx, char **argv)
{
	t_envnode	*node;
	char		*eq;
	char		*key;
	char		*val;

	if (argv && argv[1])
	{
		eq = ft_strchr(argv[1], '=');
		if (eq)
		{
			key = ft_substr(argv[1], 0, (unsigned int)(eq - argv[1]));
			if (!key)
				return (-1);
			val = ft_strdup(eq + 1);
			if (!val)
			{
				free(key);
				return (-1);
			}
			ms_env_set(ctx, key, val, true);
			free(key);
			free(val);
		}
		else
		{
			ft_putstr_fd("env: '", STDERR_FILENO);
			ft_putstr_fd(argv[1], STDERR_FILENO);
			ft_putendl_fd("': No such file or directory", STDERR_FILENO);
			return (127);
		}
	}
	
	node = ctx->env;
	while (node)
	{
		if (node->has_val)
		{
			ft_putstr_fd(node->key, STDOUT_FILENO);
			ft_putstr_fd("=", STDOUT_FILENO);
			if (node->val)
				ft_putstr_fd(node->val, STDOUT_FILENO);
			ft_putstr_fd("\n", STDOUT_FILENO);
		}
		node = node->next;
	}
	return (0);
}
