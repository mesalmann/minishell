/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hdere <hdere@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/08 12:00:00 by hdere             #+#    #+#             */
/*   Updated: 2026/03/08 07:44:55 by hdere            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static bool	load_env_entry(t_ctx *ctx, char *entry)
{
	char	*eq;
	char	*key;
	char	*val;

	eq = strchr(entry, '=');
	if (!eq)
		return (ms_env_set(ctx, entry, NULL, false));
	key = ft_substr(entry, 0, (size_t)(eq - entry));
	val = ft_strdup(eq + 1);
	if (!key || !val)
	{
		free(key);
		free(val);
		return (false);
	}
	if (!ms_env_set(ctx, key, val, true))
	{
		free(key);
		free(val);
		return (false);
	}
	free(key);
	free(val);
	return (true);
}

static bool	load_env(t_ctx *ctx, char **envp)
{
	int	i;

	i = 0;
	while (envp[i])
	{
		if (!load_env_entry(ctx, envp[i]))
			return (false);
		i++;
	}
	return (true);
}

static void	increment_shlvl(t_ctx *ctx)
{
	char	*val;
	int		level;
	char	*new_val;

	val = ms_env_get(ctx, "SHLVL");
	if (val)
		level = atoi(val) + 1;
	else
		level = 1;
	if (level < 0)
		level = 0;
	new_val = ft_itoa(level);
	if (new_val)
	{
		ms_env_set(ctx, "SHLVL", new_val, true);
		free(new_val);
	}
}

bool	ms_ctx_init(t_ctx *ctx, char **envp)
{
	ft_memset(ctx, 0, sizeof(t_ctx));
	ctx->interactive = isatty(STDIN_FILENO);
	ctx->last_status = 0;
	ctx->env_dirty = true;
	ctx->env = NULL;
	ctx->envp_cache = NULL;
	if (envp && *envp)
	{
		if (!load_env(ctx, envp))
			return (false);
	}
	increment_shlvl(ctx);
	return (true);
}
