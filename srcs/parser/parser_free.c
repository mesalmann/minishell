/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_free.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hdere <hdere@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/08 12:00:00 by hdere             #+#    #+#             */
/*   Updated: 2026/03/08 07:49:16 by hdere            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static void free_redir_list(t_redir *r)
{
    t_redir *tmp;

    while (r)
    {
        tmp = r->next;
        free(r->target);
        free(r);
        r = tmp;
    }
}

static void free_heredoc_list(t_heredoc *h)
{
    t_heredoc *tmp;

    while (h)
    {
        tmp = h->next;
        if (h->pipe_rd >= 0)
            close(h->pipe_rd);
        if (h->pipe_wr >= 0)
            close(h->pipe_wr);
        free(h->delim);
        free(h);
        h = tmp;
    }
}

void ms_cmd_free(t_cmdnode *cmd)
{
    int i;

    if (!cmd)
        return;
    if (cmd->argv)
    {
        i = 0;
        while (cmd->argv[i])
            free(cmd->argv[i++]);
        free(cmd->argv);
    }
    free_redir_list(cmd->redirs);
    free_heredoc_list(cmd->heredocs);
    free(cmd);
}

void ms_cmd_free_list(t_cmdnode *head)
{
    t_cmdnode *tmp;

    while (head)
    {
        tmp = head->next;
        ms_cmd_free(head);
        head = tmp;
    }
}
