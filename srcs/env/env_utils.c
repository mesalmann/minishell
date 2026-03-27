/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   env_utils.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hdere <hdere@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/08 12:00:00 by hdere             #+#    #+#             */
/*   Updated: 2026/03/26 20:24:18 by hdere            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

t_envnode	*node_new(const char *key, const char *val, bool has_val)
{
	t_envnode	*node;

	node = malloc(sizeof(t_envnode));
	if (!node)
		return (NULL);
	node->key = ft_strdup(key);
	node->next = NULL;
	node->has_val = has_val;
	if (has_val && val)
		node->val = ft_strdup(val);
	else if (has_val)
		node->val = ft_strdup("");
	else
		node->val = NULL;
	if (!node->key || (has_val && !node->val))
	{
		free(node->key);
		free(node->val);
		free(node);
		return (NULL);
	}
	return (node);
}

void	_sigblock_env(sigset_t *new_set, sigset_t *old_set)
{
	sigemptyset(new_set);
	sigaddset(new_set, SIGINT);
	sigaddset(new_set, SIGTERM);
	sigprocmask(SIG_BLOCK, new_set, old_set);
}

char	*get_val_str(const char *val)
{
	if (val)
		return (ft_strdup(val));
	return (ft_strdup(""));
}
