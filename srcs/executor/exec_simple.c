/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_simple.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hdere <hdere@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/08 12:00:00 by hdere             #+#    #+#             */
/*   Updated: 2026/03/08 07:34:34 by hdere            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "executor_internal.h"

static char *join_path(char *dir, char *cmd)
{
	char *temp;
	char *full_path;

	temp = ft_strjoin(dir, "/");
	if (!temp)
		return (NULL);
	full_path = ft_strjoin(temp, cmd);
	free(temp);
	return (full_path);
}

static char *find_direct_path(char *cmd)
{
	struct stat	st;

	if (access(cmd, F_OK) != 0)
		return (NULL);
	if (stat(cmd, &st) == 0 && S_ISDIR(st.st_mode))
	{
		errno = EISDIR;
		return (NULL);
	}
	if (access(cmd, X_OK) != 0)
		return (NULL);
	return (ft_strdup(cmd));
}

static char *search_in_paths(char **paths, char *cmd, int *perm)
{
	char *full;
	int i;

	i = 0;
	while (paths[i])
	{
		full = join_path(paths[i], cmd);
		if (access(full, F_OK) == 0)
		{
			if (access(full, X_OK) == 0)
				return (full);
			*perm = 1;
		}
		free(full);
		i++;
	}
	return (NULL);
}

static char *ms_find_path(char *cmd, char **envp)
{
	int i;
	char **paths;
	char *result;
	int perm;

	perm = 0;
	if (ft_strchr(cmd, '/'))
		return (find_direct_path(cmd));
	i = 0;
	while (envp[i] && ft_strncmp(envp[i], "PATH=", 5) != 0)
		i++;
	if (!envp[i])
	{
		errno = ENOENT;
		return (NULL);
	}
	paths = ft_split(envp[i] + 5, ':');
	if (!paths)
		return (NULL);
	result = search_in_paths(paths, cmd, &perm);
	free_tab(paths);
	if (!result && perm)
		errno = EACCES;
	else if (!result)
		errno = ENOENT;
	return (result);
}

char *ms_resolve_path(t_ctx *ctx, const char *file)
{
	if (ctx->env_dirty && !ms_env_build_envp(ctx))
		return (NULL);
	return (ms_find_path((char *)file, ctx->envp_cache));
}
