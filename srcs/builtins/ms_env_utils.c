/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ms_env_utils.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hdere <hdere@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/08 12:00:00 by hdere             #+#    #+#             */
/*   Updated: 2026/03/08 07:37:15 by hdere            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static void	env_print_line(const char *key, const char *val)
{
	ft_putstr_fd((char *)key, STDOUT_FILENO);
	ft_putstr_fd("=", STDOUT_FILENO);
	if (val)
		ft_putstr_fd((char *)val, STDOUT_FILENO);
	ft_putstr_fd("\n", STDOUT_FILENO);
}

void	env_print_with_overrides(t_ctx *ctx, t_envnode *temp)
{
	t_envnode	*node;
	t_envnode	*over;
	bool		printed;

	node = ctx->env;
	while (node)
	{
		over = temp;
		printed = false;
		while (over)
		{
			if (ft_strcmp(over->key, node->key) == 0)
			{
				printed = true;
				env_print_line(over->key, over->val);
			}
			if (printed)
				break ;
			over = over->next;
		}
		if (!printed && node->has_val)
			env_print_line(node->key, node->val);
		node = node->next;
	}
}

void	env_print_remain(t_ctx *ctx, t_envnode *temp)
{
	t_envnode	*over;
	t_envnode	*check;
	bool		printed;

	over = temp;
	while (over)
	{
		check = ctx->env;
		printed = false;
		while (check)
		{
			if (ft_strcmp(check->key, over->key) == 0)
			{
				printed = true;
				break ;
			}
			check = check->next;
		}
		if (!printed)
			env_print_line(over->key, over->val);
		over = over->next;
	}
}

int	env_arg_error(char *arg)
{
	ft_putstr_fd("minishell: env: '", STDERR_FILENO);
	ft_putstr_fd(arg, STDERR_FILENO);
	ft_putendl_fd("': not a valid identifier", STDERR_FILENO);
	return (1);
}
