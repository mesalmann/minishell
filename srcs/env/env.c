/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   env.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hdere <hdere@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/08 12:00:00 by hdere             #+#    #+#             */
/*   Updated: 2026/03/08 07:34:34 by hdere            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include <string.h>

/* ── node allocation ─────────────────────────────────────────────────────── */

static t_envnode *node_new(const char *key, const char *val, bool has_val)
{
	t_envnode *node;

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

/* ── helpers ─────────────────────────────────────────────────────────────── */

static char *get_val_str(const char *val)
{
	if (val)
		return (ft_strdup(val));
	return (ft_strdup(""));
}

static bool env_update_node(t_envnode *nd, const char *val, bool hv, t_ctx *c)
{
	char *new_val;

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

/* ── ms_env_set ──────────────────────────────────────────────────────────── */

bool ms_env_set(t_ctx *ctx, const char *key, const char *val, bool has_val)
{
	t_envnode *node;
	t_envnode *prev;

	node = ctx->env;
	prev = NULL;
	while (node)
	{
		if (strcmp(node->key, key) == 0)
			return (env_update_node(node, val, has_val, ctx));
		prev = node;
		node = node->next;
	}
	node = node_new(key, val, has_val);
	if (!node)
		return (false);
	if (prev)
		prev->next = node;
	else
		ctx->env = node;
	ctx->env_dirty = true;
	return (true);
}

/* ── ms_env_unset ────────────────────────────────────────────────────────── */

bool ms_env_unset(t_ctx *ctx, const char *key)
{
	t_envnode *node;
	t_envnode *prev;

	node = ctx->env;
	prev = NULL;
	while (node)
	{
		if (strcmp(node->key, key) == 0)
		{
			if (prev)
				prev->next = node->next;
			else
				ctx->env = node->next;
			free(node->key);
			free(node->val);
			free(node);
			ctx->env_dirty = true;
			return (true);
		}
		prev = node;
		node = node->next;
	}
	return (true);
}
