#include "../minishell.h"
#include <string.h>

/*
** load_env - char **envp'yi parse ederek ctx->env linked list'ine yükler.
** Her giriş "KEY=VALUE" formatındadır (standart envp garantisi).
** '=' yoksa: anahtar olarak ekler, has_val=false (güvenli fallback).
*/
static bool	load_env(t_ctx *ctx, char **envp)
{
	int		i;
	char	*eq;
	char	*key;
	char	*val;

	i = 0;
	while (envp[i])
	{
		eq = strchr(envp[i], '=');
		if (eq)
		{
			key = ft_substr(envp[i], 0, (size_t)(eq - envp[i]));
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
		}
		else
		{
			if (!ms_env_set(ctx, envp[i], NULL, false))
				return (false);
		}
		i++;
	}
	return (true);
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
	return (true);
}

/*
** ms_ctx_destroy - ctx'nin tüm dinamik kaynaklarını serbest bırakır.
** main() çıkışında çağrılmalıdır.
*/
void	ms_ctx_destroy(t_ctx *ctx)
{
	t_envnode	*node;
	t_envnode	*tmp;

	if (!ctx)
		return ;
	node = ctx->env;
	while (node)
	{
		tmp = node->next;
		free(node->key);
		free(node->val);
		free(node);
		node = tmp;
	}
	ctx->env = NULL;
	free_tab(ctx->envp_cache);
	ctx->envp_cache = NULL;
}
