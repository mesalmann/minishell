/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_cmd.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hdere <hdere@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/08 12:00:00 by hdere             #+#    #+#             */
/*   Updated: 2026/03/08 07:46:32 by hdere            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "parser_internal.h"

t_cmdnode *ms_cmd_new(void)
{
    t_cmdnode *c;

    c = malloc(sizeof(t_cmdnode));
    if (!c)
        return (NULL);
    c->argv = NULL;
    c->redirs = NULL;
    c->heredocs = NULL;
    c->next = NULL;
    return (c);
}

t_redir *ms_redir_new(t_redirtype type, char *target)
{
    t_redir *r;

    r = malloc(sizeof(t_redir));
    if (!r)
        return (NULL);
    r->type = type;
    r->target = target;
    r->next = NULL;
    return (r);
}

t_heredoc *ms_heredoc_new(char *delim, bool expand_mode)
{
    t_heredoc *h;

    h = malloc(sizeof(t_heredoc));
    if (!h)
        return (NULL);
    h->delim = delim;
    h->expand_mode = expand_mode;
    h->pipe_rd = -1;
    h->pipe_wr = -1;
    h->next = NULL;
    return (h);
}

void ms_redir_add_back(t_redir **lst, t_redir *new)
{
    t_redir *t;

    if (!lst || !new)
        return;
    if (!*lst)
    {
        *lst = new;
        return;
    }
    t = *lst;
    while (t->next)
        t = t->next;
    t->next = new;
}

void ms_heredoc_add_back(t_heredoc **lst, t_heredoc *new)
{
    t_heredoc *t;

    if (!lst || !new)
        return;
    if (!*lst)
    {
        *lst = new;
        return;
    }
    t = *lst;
    while (t->next)
        t = t->next;
    t->next = new;
}
