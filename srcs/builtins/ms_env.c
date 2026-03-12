/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ms_env.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hdere <hdere@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/08 12:00:00 by hdere             #+#    #+#             */
/*   Updated: 2026/03/08 07:08:53 by hdere            ###   ########.fr       */
/*                                                                        	    */
/* ************************************************************************** */

#include "minishell.h"

static bool env_add_temp(t_envnode **temp, const char *key, const char *val)
{
	t_envnode *node;
	t_envnode *curr;

	node = malloc(sizeof(t_envnode));
	if (!node)
		return (false);
	node->key = ft_strdup(key);
	node->val = ft_strdup(val);
	node->has_val = true;
	node->next = NULL;
	if (!node->key || !node->val)
	{
		free(node->key);
		free(node->val);
		free(node);
		return (false);
	}
	if (!(*temp))
		*temp = node;
	else
	{
		curr = *temp;
		while (curr->next)
			curr = curr->next;
		curr->next = node;
	}
	return (true);
}

static void env_free_temp(t_envnode *temp)
{
	t_envnode *next;

	while (temp)
	{
		next = temp->next;
		free(temp->key);
		free(temp->val);
		free(temp);
		temp = next;
	}
}

static void env_print_with_overrides(t_ctx *ctx, t_envnode *temp)
{
	t_envnode *node;
	t_envnode *override;
	t_envnode *check;
	bool printed;

	node = ctx->env;
	while (node)
	{
		override = temp;
		printed = false;
		while (override)
		{
			if (strcmp(override->key, node->key) == 0)
			{
				ft_putstr_fd(override->key, STDOUT_FILENO);
				ft_putstr_fd("=", STDOUT_FILENO);
				ft_putstr_fd(override->val, STDOUT_FILENO);
				ft_putstr_fd("\n", STDOUT_FILENO);
				printed = true;
				break ;
			}
			override = override->next;
		}
		if (!printed && node->has_val)
		{
			ft_putstr_fd(node->key, STDOUT_FILENO);
			ft_putstr_fd("=", STDOUT_FILENO);
			if (node->val)
				ft_putstr_fd(node->val, STDOUT_FILENO);
			ft_putstr_fd("\n", STDOUT_FILENO);
		}
		node = node->next;
	}
	override = temp;
	while (override)
	{
		check = ctx->env;
		printed = false;
		while (check)
		{
			if (strcmp(check->key, override->key) == 0)
			{
				printed = true;
				break ;
			}
			check = check->next;
		}
		if (!printed)
		{
			ft_putstr_fd(override->key, STDOUT_FILENO);
			ft_putstr_fd("=", STDOUT_FILENO);
			ft_putstr_fd(override->val, STDOUT_FILENO);
			ft_putstr_fd("\n", STDOUT_FILENO);
		}
		override = override->next;
	}
}

int ms_builtin_env(t_ctx *ctx, char **argv)
{
	t_envnode	*temp;
	t_envnode	*curr;
	char		*eq;
	char		*key;
	int			i;
	int			cmd_idx;
	pid_t		pid;
	int			status;

	temp = NULL;
	i = 1;
	cmd_idx = -1;
	while (argv && argv[i])
	{
		eq = ft_strchr(argv[i], '=');
		if (eq)
		{
			key = ft_substr(argv[i], 0, (unsigned int)(eq - argv[i]));
			if (!key || !ms_is_valid_identifier(key))
			{
				free(key);
				env_free_temp(temp);
				ft_putstr_fd("minishell: env: '", STDERR_FILENO);
				ft_putstr_fd(argv[i], STDERR_FILENO);
				ft_putendl_fd("': not a valid identifier", STDERR_FILENO);
				return (1);
			}
			if (!env_add_temp(&temp, key, eq + 1))
			{
				free(key);
				env_free_temp(temp);
				return (-1);
			}
			free(key);
			i++;
		}
		else
		{
			cmd_idx = i;
			break ;
		}
	}
	if (cmd_idx == -1)
	{
		env_print_with_overrides(ctx, temp);
		env_free_temp(temp);
		return (0);
	}
	pid = fork();
	if (pid < 0)
	{
		env_free_temp(temp);
		perror("minishell: fork");
		return (1);
	}
	if (pid == 0)
	{
		curr = temp;
		while (curr)
		{
			ms_env_set(ctx, curr->key, curr->val, true);
			curr = curr->next;
		}
		ms_builtin_run_argv(ctx, &argv[cmd_idx]);
		status = ctx->last_status;
		env_free_temp(temp);
		ms_ctx_destroy(ctx);
		_exit(status);
	}
	while (waitpid(pid, &status, 0) == -1 && errno == EINTR)
		continue ;
	if (WIFEXITED(status))
		ctx->last_status = WEXITSTATUS(status);
	else if (WIFSIGNALED(status))
	{
		ctx->last_status = 128 + WTERMSIG(status);
		ms_print_signal_msg(status);
	}
	env_free_temp(temp);
	return (ctx->last_status);
}
