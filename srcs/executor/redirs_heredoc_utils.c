/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   redirs_heredoc_utils.c                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hdere <hdere@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/08 12:00:00 by hdere             #+#    #+#             */
/*   Updated: 2026/03/26 20:38:44 by hdere            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "executor_internal.h"

bool	write_hd_line(t_ctx *ctx, t_heredoc *h, char *line)
{
	char	*expanded;
	size_t	len;

	if (h->expand_mode)
	{
		expanded = ms_expand_str(ctx, line, true);
		if (!expanded)
			return (false);
		len = ft_strlen(expanded);
		write(h->pipe_wr, expanded, len);
		write(h->pipe_wr, "\n", 1);
		free(expanded);
	}
	else
	{
		len = ft_strlen(line);
		write(h->pipe_wr, line, len);
		write(h->pipe_wr, "\n", 1);
	}
	return (true);
}

static int	check_sigint_and_read(char **line)
{
	if (g_sig == SIGINT)
	{
		g_sig = 0;
		return (-1);
	}
	*line = readline("> ");
	if (g_sig == SIGINT)
	{
		free(*line);
		g_sig = 0;
		return (-1);
	}
	return (1);
}

static int	hd_process_input(t_ctx *ctx, t_heredoc *h)
{
	char	*line;
	int		ret;

	if (check_sigint_and_read(&line) == -1)
		return (-1);
	if (!line)
	{
		ft_putstr_fd("minishell: warning: here-document at EOF\n", 2);
		return (0);
	}
	if (ft_strncmp(line, h->delim, ft_strlen(h->delim) + 1) == 0)
	{
		free(line);
		return (0);
	}
	ret = 1;
	if (!write_hd_line(ctx, h, line))
		ret = -1;
	free(line);
	return (ret);
}

bool	read_one_heredoc(t_ctx *ctx, t_heredoc *h)
{
	int	pipefd[2];
	int	ret;

	if (pipe(pipefd) == -1)
	{
		perror("minishell: pipe");
		return (false);
	}
	h->pipe_rd = pipefd[0];
	h->pipe_wr = pipefd[1];
	ret = 1;
	while (ret == 1)
		ret = hd_process_input(ctx, h);
	close(h->pipe_wr);
	h->pipe_wr = -1;
	if (ret == -1)
		return (false);
	return (true);
}

void	hd_close_open_pipes(t_cmdnode *pipeline)
{
	t_cmdnode	*cmd;
	t_heredoc	*h;

	cmd = pipeline;
	while (cmd)
	{
		h = cmd->heredocs;
		while (h)
		{
			if (h->pipe_rd >= 0)
			{
				close(h->pipe_rd);
				h->pipe_rd = -1;
			}
			if (h->pipe_wr >= 0)
			{
				close(h->pipe_wr);
				h->pipe_wr = -1;
			}
			h = h->next;
		}
		cmd = cmd->next;
	}
}
