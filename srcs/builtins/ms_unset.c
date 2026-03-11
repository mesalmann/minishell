/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ms_unset.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hdere <hdere@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/08 12:00:00 by hdere             #+#    #+#             */
/*   Updated: 2026/03/08 07:09:15 by hdere            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static int	unset_invalid_opt(const char *arg)
{
	ft_putstr_fd("minishell: unset: -", STDERR_FILENO);
	write(STDERR_FILENO, &arg[1], 1);
	ft_putendl_fd(": invalid option", STDERR_FILENO);
	return (2);
}

int	ms_builtin_unset(t_ctx *ctx, char **argv)
{
	int	i;
	int	ret;

	ret = 0;
	i = 1;
	while (argv[i])
	{
		if (argv[i][0] == '-' && argv[i][1])
			ret = unset_invalid_opt(argv[i]);
		else if (ms_is_valid_identifier(argv[i]))
			ms_env_unset(ctx, argv[i]);
		i++;
	}
	return (ret);
}
