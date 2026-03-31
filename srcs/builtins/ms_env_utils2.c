/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ms_env_utils2.c                                     :+:      :+:    :+:  */
/*                                                    +:+ +:+         +:+     */
/*   By: hdere <hdere@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/08 12:00:00 by hdere             #+#    #+#             */
/*   Updated: 2026/03/08 07:37:15 by hdere            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	env_free_temp(t_envnode *temp)
{
	t_envnode	*next;

	while (temp)
	{
		next = temp->next;
		free(temp->key);
		free(temp->val);
		free(temp);
		temp = next;
	}
}

int	parse_env_args(char **argv, t_envnode **temp, int *cmd_idx)
{
	int		i;
	char	*eq;
	char	*key;

	i = 0;
	while (argv && argv[++i])
	{
		eq = ft_strchr(argv[i], '=');
		if (!eq)
			break ;
		key = ft_substr(argv[i], 0, (unsigned int)(eq - argv[i]));
		if (!key || !ms_is_valid_identifier(key))
		{
			free(key);
			return (env_arg_error(argv[i]));
		}
		if (!env_add_temp(temp, key, eq + 1))
		{
			free(key);
			return (-1);
		}
		free(key);
	}
	return (*cmd_idx = i, (void)(!argv[i] && (*cmd_idx = -1)), 0);
}

bool	env_add_temp(t_envnode **temp, const char *key, const char *val)
{
	t_envnode	*node;
	t_envnode	*curr;

	node = malloc(sizeof(t_envnode));
	if (!node)
		return (false);
	node->key = ft_strdup(key);
	node->val = ft_strdup(val);
	node->has_val = true;
	node->next = NULL;
	if (!node->key || !node->val)
	{
		(free(node->key), free(node->val), free(node));
		return (false);
	}
	if (!(*temp))
		return (*temp = node, true);
	curr = *temp;
	while (curr->next)
		curr = curr->next;
	curr->next = node;
	return (true);
}
