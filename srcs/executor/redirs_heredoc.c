/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   redirs_heredoc.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hdere <hdere@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/08 12:00:00 by hdere             #+#    #+#             */
/*   Updated: 2026/03/08 07:43:45 by hdere            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "executor_internal.h"

static void	close_all_heredoc_rds(t_cmdnode *cmd)
{
	t_heredoc	*h;

	h = cmd->heredocs;
	while (h)
	{
		if (h->pipe_rd >= 0)
			close(h->pipe_rd);
		h->pipe_rd = -1;
		h = h->next;
	}
}

bool	ms_apply_heredoc_redir(t_cmdnode *cmd)
{
	t_heredoc	*h;
	t_heredoc	*last;
	bool		ok;

	last = NULL;
	ok = true;
	h = cmd->heredocs;
	while (h)
	{
		last = h;
		h = h->next;
	}
	if (last && last->pipe_rd >= 0
		&& dup2(last->pipe_rd, STDIN_FILENO) == -1)
		ok = false;
	close_all_heredoc_rds(cmd);
	if (!ok)
		perror("minishell: dup2");
	return (ok);
}

static bool	hd_fail_cleanup(t_cmdnode *pipe, int saved_in)
{
	hd_close_open_pipes(pipe);
	dup2(saved_in, STDIN_FILENO);
	close(saved_in);
	ms_sig_install_interactive();
	return (false);
}

static bool	process_pipeline_heredocs(t_ctx *ctx, t_cmdnode *pipe, int s_in)
{
	t_heredoc	*h;

	while (pipe)
	{
		h = pipe->heredocs;
		while (h)
		{
			if (!read_one_heredoc(ctx, h))
				return (hd_fail_cleanup(pipe, s_in));
			h = h->next;
		}
		pipe = pipe->next;
	}
	return (true);
}

bool	ms_run_heredocs(t_ctx *ctx, t_cmdnode *pipeline)
{
	int	saved_stdin;

	saved_stdin = dup(STDIN_FILENO);
	if (saved_stdin < 0)
	{
		ft_putstr_fd("minishell: cannot save stdin\n", 2);
		return (false);
	}
	ms_sig_install_heredoc();
	if (!process_pipeline_heredocs(ctx, pipeline, saved_stdin))
		return (false);
	dup2(saved_stdin, STDIN_FILENO);
	close(saved_stdin);
	ms_sig_install_interactive();
	return (true);
}
