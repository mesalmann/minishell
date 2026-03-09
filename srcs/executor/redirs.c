/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   redirs.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hdere <hdere@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/08 12:00:00 by hdere             #+#    #+#             */
/*   Updated: 2026/03/08 07:43:12 by hdere            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static int open_redir_fd(t_redir *r)
{
	if (r->type == RD_IN)
		return (open(r->target, O_RDONLY));
	if (r->type == RD_OUT_TRUNC)
		return (open(r->target, O_WRONLY | O_CREAT | O_TRUNC, 0644));
	return (open(r->target, O_WRONLY | O_CREAT | O_APPEND, 0644));
}

static void restore_and_clear(int *saved_stdin, int *saved_stdout)
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

static int apply_one_redir(t_redir *r)
{
	int fd;
	int tfd;

	fd = open_redir_fd(r);
	if (fd == -1)
	{
		ft_putstr_fd("minishell: ", 2);
		ft_putstr_fd(r->target, 2);
		ft_putstr_fd(": ", 2);
		ft_putendl_fd(strerror(errno), 2);
		return (0);
	}
	if (r->type == RD_IN)
		tfd = STDIN_FILENO;
	else
		tfd = STDOUT_FILENO;
	if (dup2(fd, tfd) == -1)
	{
		perror("minishell: dup2");
		close(fd);
		return (0);
	}
	close(fd);
	return (1);
}

bool ms_apply_redirs(t_cmdnode *cmd, int *sin, int *sout)
{
	t_redir *r;

	if (sin)
		*sin = dup(STDIN_FILENO);
	if (sout)
		*sout = dup(STDOUT_FILENO);
	r = cmd->redirs;
	while (r)
	{
		if (!apply_one_redir(r))
		{
			restore_and_clear(sin, sout);
			return (false);
		}
		r = r->next;
	}
	if (!ms_apply_heredoc_redir(cmd))
	{
		restore_and_clear(sin, sout);
		return (false);
	}
	return (true);
}

void ms_restore_stdio(int saved_stdin, int saved_stdout)
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
