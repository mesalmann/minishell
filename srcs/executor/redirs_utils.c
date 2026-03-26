/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   redirs_utils.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hdere <hdere@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/08 12:00:00 by hdere             #+#    #+#             */
/*   Updated: 2026/03/26 20:38:30 by hdere            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "executor_internal.h"

void	restore_and_clear(int *saved_stdin, int *saved_stdout)
{
	if (saved_stdin && *saved_stdin >= 0)
	{
		dup2(*saved_stdin, STDIN_FILENO);
		close(*saved_stdin);
		*saved_stdin = -1;
	}
	if (saved_stdout && *saved_stdout >= 0)
	{
		dup2(*saved_stdout, STDOUT_FILENO);
		close(*saved_stdout);
		*saved_stdout = -1;
	}
}

bool	save_stdio(int *sin, int *sout)
{
	if (sin)
	{
		*sin = dup(STDIN_FILENO);
		if (*sin == -1)
		{
			perror("minishell: dup(stdin)");
			return (false);
		}
	}
	if (sout)
	{
		*sout = dup(STDOUT_FILENO);
		if (*sout == -1)
		{
			if (sin && *sin >= 0)
			{
				close(*sin);
				*sin = -1;
			}
			perror("minishell: dup(stdout)");
			return (false);
		}
	}
	return (true);
}

void	ms_restore_stdio(int saved_stdin, int saved_stdout)
{
	if (saved_stdin >= 0)
	{
		dup2(saved_stdin, STDIN_FILENO);
		close(saved_stdin);
	}
	if (saved_stdout >= 0)
	{
		dup2(saved_stdout, STDOUT_FILENO);
		close(saved_stdout);
	}
}
