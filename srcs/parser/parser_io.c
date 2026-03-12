/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_io.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hdere <hdere@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/08 12:00:00 by hdere             #+#    #+#             */
/*   Updated: 2026/03/08 07:48:43 by hdere            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

bool ms_is_redir_or_heredoc(t_opkind op)
{
    if (op == OP_IN || op == OP_OUT_TRUNC)
        return (true);
    if (op == OP_OUT_APPEND || op == OP_HEREDOC)
        return (true);
    return (false);
}

static bool parse_one_redir(t_cmdnode *cmd, t_token **t)
{
    t_redirtype rt;
    t_redir *r;
    char *target;

    rt = RD_IN;
    if ((*t)->op == OP_OUT_TRUNC)
        rt = RD_OUT_TRUNC;
    else if ((*t)->op == OP_OUT_APPEND)
        rt = RD_OUT_APPEND;
    *t = (*t)->next;
    if (!*t || (*t)->kind != TK_WORD)
        return (false);
    target = ft_strdup((*t)->lex);
    if (!target)
        return (false);
    r = ms_redir_new(rt, target);
    if (!r)
    {
        free(target);
        return (false);
    }
    ms_redir_add_back(&cmd->redirs, r);
    *t = (*t)->next;
    return (true);
}

static bool parse_one_heredoc(t_cmdnode *cmd, t_token **t)
{
    char *delim;
    t_heredoc *h;

    *t = (*t)->next;
    if (!*t || (*t)->kind != TK_WORD)
        return (false);
    delim = ft_strdup((*t)->lex);
    if (!delim)
        return (false);
    h = ms_heredoc_new(delim, !(*t)->no_expand);
    if (!h)
    {
        free(delim);
        return (false);
    }
    ms_heredoc_add_back(&cmd->heredocs, h);
    *t = (*t)->next;
    return (true);
}

bool ms_parse_ios(t_cmdnode *cmd, t_token *t)
{
    while (t && !(t->kind == TK_OP && t->op == OP_PIPE))
    {
        if (t->kind == TK_OP && ms_is_redir_or_heredoc(t->op))
        {
            if (t->op == OP_HEREDOC)
            {
                if (!parse_one_heredoc(cmd, &t))
                    return (false);
            }
            else
            {
                if (!parse_one_redir(cmd, &t))
                    return (false);
            }
            continue;
        }
        t = t->next;
    }
    return (true);
}
