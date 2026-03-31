/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   env.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hdere <hdere@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/08 12:00:00 by hdere             #+#    #+#             */
/*   Updated: 2026/03/26 21:36:10 by hdere            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include "env_internal.h"

static bool	env_update_node(t_envnode *nd, const char *val, bool hv, t_ctx *c)
{
	char	*new_val;

	if (hv)
	{
		new_val = get_val_str(val);
		if (!new_val)
			return (false);
		free(nd->val);
		nd->val = new_val;
		nd->has_val = true;
	}
	c->env_dirty = true;
	return (true);
}

bool	ms_env_set(t_ctx *ctx, const char *key, const char *val, bool has_val)
{
	t_envnode	*node;
	t_envnode	*prev;
	sigset_t	old;
	sigset_t	new;

	node = ctx->env;
	prev = NULL;
	while (node)
	{
		if (ft_strncmp(node->key, key, ft_strlen(key) + 1) == 0)
			return (env_update_node(node, val, has_val, ctx));
		prev = node;
		node = node->next;
	}
	node = node_new(key, val, has_val);
	if (!node)
		return (false);
	_sigblock_env(&new, &old);
	if (prev)
		prev->next = node;
	else
		ctx->env = node;
	ctx->env_dirty = true;
	sigprocmask(SIG_SETMASK, &old, NULL);
	return (true);
}

static void	remove_env_node(t_ctx *ctx, t_envnode *node, t_envnode *prev)
{
	sigset_t	old;
	sigset_t	new;

	_sigblock_env(&new, &old);
	if (prev)
		prev->next = node->next;
	else
		ctx->env = node->next;
	sigprocmask(SIG_SETMASK, &old, NULL);
	free(node->key);
	free(node->val);
	free(node);
	ctx->env_dirty = true;
}

bool	ms_env_unset(t_ctx *ctx, const char *key)
{
	t_envnode	*node;
	t_envnode	*prev;

	node = ctx->env;
	prev = NULL;
	while (node)
	{
		if (ft_strncmp(node->key, key, ft_strlen(key) + 1) == 0)
		{
			remove_env_node(ctx, node, prev);
			return (true);
		}
		prev = node;
		node = node->next;
	}
	return (true);
}

char	*ms_env_get(t_ctx *ctx, const char *key)
{
	t_envnode	*curr;
	size_t		len;

	if (!ctx || !ctx->env || !key)
		return (NULL);
	curr = ctx->env;
	len = ft_strlen(key);
	while (curr)
	{
		if (ft_strncmp(curr->key, key, len) == 0 && curr->key[len] == '\0')
		{
			if (curr->has_val)
				return (curr->val);
			return (NULL);
		}
		curr = curr->next;
	}
	return (NULL);
}
