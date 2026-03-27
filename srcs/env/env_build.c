/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   env_build.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hdere <hdere@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/08 12:00:00 by hdere             #+#    #+#             */
/*   Updated: 2026/03/26 20:24:54 by hdere            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static int	count_env_vals(t_envnode *env)
{
	int	count;

	count = 0;
	while (env)
	{
		if (env->has_val)
			count++;
		env = env->next;
	}
	return (count);
}

static char	*build_envp_entry(t_envnode *node)
{
	char	*tmp;
	char	*kv;

	tmp = ft_strjoin(node->key, "=");
	if (!tmp)
		return (NULL);
	if (node->val)
		kv = ft_strjoin(tmp, node->val);
	else
		kv = ft_strjoin(tmp, "");
	free(tmp);
	return (kv);
}

static char	**fill_envp_array(t_envnode *env, char **arr)
{
	int	i;

	i = 0;
	while (env)
	{
		if (env->has_val)
		{
			arr[i] = build_envp_entry(env);
			if (!arr[i])
			{
				arr[i] = NULL;
				free_tab(arr);
				return (NULL);
			}
			i++;
		}
		env = env->next;
	}
	arr[i] = NULL;
	return (arr);
}

char	**ms_env_build_envp(t_ctx *ctx)
{
	int		count;
	char	**arr;

	count = count_env_vals(ctx->env);
	arr = malloc(sizeof(char *) * (count + 1));
	if (!arr)
		return (NULL);
	arr = fill_envp_array(ctx->env, arr);
	if (!arr)
		return (NULL);
	free_tab(ctx->envp_cache);
	ctx->envp_cache = arr;
	ctx->env_dirty = false;
	return (arr);
}
