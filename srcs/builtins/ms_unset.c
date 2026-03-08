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

int	ms_builtin_unset(t_ctx *ctx, char **argv)
{
	int	i;
	int	ret;

	ret = 0;
	i = 1;
	while (argv[i])
	{
		if (!ms_is_valid_identifier(argv[i]))
		{
			ft_putstr_fd("minishell: unset: `", STDERR_FILENO);
			ft_putstr_fd(argv[i], STDERR_FILENO);
			ft_putendl_fd("': not a valid identifier", STDERR_FILENO);
			ret = 1;
		}
		else
			ms_env_unset(ctx, argv[i]);
		i++;
	}
	return (ret);
}
