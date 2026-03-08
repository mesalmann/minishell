/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ms_env.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hdere <hdere@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/08 12:00:00 by hdere             #+#    #+#             */
/*   Updated: 2026/03/08 07:08:53 by hdere            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../minishell.h"

/*
** ms_builtin_env - shell'in env listesini KEY=VALUE formatında basar.
**
** Kural:
**   - Sadece has_val == true olan node'lar basılır.
**     (export KEY → has_val=false → env çıktısına girmez)
**   - value NULL ise KEY= olarak basılır (boş değer, yine de geçerli).
**   - Argümanlar görmezden gelinir (minishell scope'u dışında).
**   - Her zaman 0 döner.
*/
int	ms_builtin_env(t_ctx *ctx)
{
	t_envnode	*node;

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
