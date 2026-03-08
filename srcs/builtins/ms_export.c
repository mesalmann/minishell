#include "../../minishell.h"

static int	env_count(t_ctx *ctx)
{
	t_envnode	*node;
	int			n;

	n = 0;
	node = ctx->env;
	while (node)
	{
		n++;
		node = node->next;
	}
	return (n);
}

static t_envnode	**env_to_arr(t_ctx *ctx, int count)
{
	t_envnode	**arr;
	t_envnode	*node;
	int			i;

	arr = malloc(sizeof(t_envnode *) * count);
	if (!arr)
		return (NULL);
	node = ctx->env;
	i = 0;
	while (node)
	{
		arr[i++] = node;
		node = node->next;
	}
	return (arr);
}

static void	sort_env_arr(t_envnode **arr, int count)
{
	int			i;
	int			j;
	t_envnode	*tmp;

	i = 0;
	while (i < count - 1)
	{
		j = 0;
		while (j < count - 1 - i)
		{
			if (strcmp(arr[j]->key, arr[j + 1]->key) > 0)
			{
				tmp = arr[j];
				arr[j] = arr[j + 1];
				arr[j + 1] = tmp;
			}
			j++;
		}
		i++;
	}
}

static void	print_export(t_ctx *ctx)
{
	int			count;
	t_envnode	**arr;
	int			i;

	count = env_count(ctx);
	if (count == 0)
		return ;
	arr = env_to_arr(ctx, count);
	if (!arr)
		return ;
	sort_env_arr(arr, count);
	i = 0;
	while (i < count)
	{
		ft_putstr_fd("declare -x ", STDOUT_FILENO);
		ft_putstr_fd(arr[i]->key, STDOUT_FILENO);
		if (arr[i]->has_val)
		{
			ft_putstr_fd("=\"", STDOUT_FILENO);
			ft_putstr_fd(arr[i]->val ? arr[i]->val : "", STDOUT_FILENO);
			ft_putstr_fd("\"", STDOUT_FILENO);
		}
		ft_putstr_fd("\n", STDOUT_FILENO);
		i++;
	}
	free(arr);
}

static int	export_invalid_id(const char *arg)
{
	ft_putstr_fd("minishell: export: `", STDERR_FILENO);
	ft_putstr_fd(arg, STDERR_FILENO);
	ft_putendl_fd("': not a valid identifier", STDERR_FILENO);
	return (1);
}

int	ms_builtin_export(t_ctx *ctx, char **argv)
{
	int		i;
	char	*eq;
	char	*key;
	int		ret;

	if (!argv[1])
	{
		print_export(ctx);
		return (0);
	}
	ret = 0;
	i = 1;
	while (argv[i])
	{
		eq = ft_strchr(argv[i], '=');
		if (eq)
		{
			key = ft_substr(argv[i], 0, (unsigned int)(eq - argv[i]));
			if (!key)
				return (1);
			if (!ms_is_valid_identifier(key))
			{
				free(key);
				ret = export_invalid_id(argv[i]);
			}
			else
			{
				ms_env_set(ctx, key, eq + 1, true);
				free(key);
			}
		}
		else
		{
			if (!ms_is_valid_identifier(argv[i]))
				ret = export_invalid_id(argv[i]);
			else
				ms_env_set(ctx, argv[i], NULL, false);
		}
		i++;
	}
	return (ret);
}
