#include "../minishell.h"
#include <string.h>

/* ── node allocation ─────────────────────────────────────────────────────── */

/*
** node_new - tek bir envnode oluşturur.
** has_val=true  → val strdup edilir (NULL ise val="" olarak saklanır).
** has_val=false → val=NULL olarak saklanır (export KEY; henüz değer yok).
*/
static t_envnode	*node_new(const char *key, const char *val, bool has_val)
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

/* ── ms_env_set ──────────────────────────────────────────────────────────── */

/*
** ms_env_set - ortam değişkeni ekle veya güncelle.
**
** has_val=true  → KEY=val   (VALUE belirlendi; env, execve envp'sinde görünür)
** has_val=false → export KEY (henüz değer yok; env'de görünmez)
**
** Varsa: val ve has_val güncellenir.
** Yoksa: listenin sonuna yeni node eklenir.
** Her durumda env_dirty=true yapılır → envp_cache sonraki execve'de rebuild edilir.
*/
bool	ms_env_set(t_ctx *ctx, const char *key, const char *val, bool has_val)
{
	t_envnode	*node;
	t_envnode	*prev;
	char		*new_val;

	node = ctx->env;
	prev = NULL;
	while (node)
	{
		if (strcmp(node->key, key) == 0)
		{
			if (has_val)
			{
				new_val = ft_strdup(val ? val : "");
				if (!new_val)
					return (false);
				free(node->val);
				node->val = new_val;
			}
			node->has_val = has_val;
			ctx->env_dirty = true;
			return (true);
		}
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

/*
** ms_env_unset - anahtarı listeden çıkar.
** Anahtar yoksa sessizce true döner (bash davranışı).
*/
bool	ms_env_unset(t_ctx *ctx, const char *key)
{
	t_envnode	*node;
	t_envnode	*prev;

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

/* ── ms_env_build_envp ───────────────────────────────────────────────────── */

/*
** ms_env_build_envp - linked list'ten execve'ye verilecek char** dizisini
** yeniden inşa eder.
**
** Kural: has_val=true olan node'lar "KEY=VALUE" formatında yer alır.
** has_val=false (export KEY) → execve envp'sine dahil edilmez.
**
** Başarıda ctx->envp_cache güncellenir, env_dirty=false yapılır.
** Malloc hatası: NULL döner, cache değişmez.
*/
char	**ms_env_build_envp(t_ctx *ctx)
{
	t_envnode	*node;
	char		**arr;
	char		*kv;
	char		*tmp;
	int			count;
	int			i;

	count = 0;
	node = ctx->env;
	while (node)
	{
		if (node->has_val)
			count++;
		node = node->next;
	}
	arr = malloc(sizeof(char *) * (count + 1));
	if (!arr)
		return (NULL);
	i = 0;
	node = ctx->env;
	while (node)
	{
		if (node->has_val)
		{
			tmp = ft_strjoin(node->key, "=");
			if (!tmp)
			{
				while (i > 0)
					free(arr[--i]);
				free(arr);
				return (NULL);
			}
			kv = ft_strjoin(tmp, node->val ? node->val : "");
			free(tmp);
			if (!kv)
			{
				while (i > 0)
					free(arr[--i]);
				free(arr);
				return (NULL);
			}
			arr[i++] = kv;
		}
		node = node->next;
	}
	arr[i] = NULL;
	free_tab(ctx->envp_cache);
	ctx->envp_cache = arr;
	ctx->env_dirty = false;
	return (arr);
}
