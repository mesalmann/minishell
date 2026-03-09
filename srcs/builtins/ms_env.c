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

#include "../../minishell.h"

int ms_builtin_env(t_ctx *ctx, char **argv)
{
	t_envnode	*node;

	if (argv && argv[1])
	{
        ft_putstr_fd("env: '", STDERR_FILENO);
        ft_putstr_fd(argv[1], STDERR_FILENO);
        ft_putendl_fd("': No such file or directory", STDERR_FILENO);
        return (127); 
    
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
