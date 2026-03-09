/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_fill.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hdere <hdere@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/08 12:00:00 by hdere             #+#    #+#             */
/*   Updated: 2026/03/08 07:49:06 by hdere            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static int count_argv_words(t_token *t)
{
    int count;

    count = 0;
    while (t && !(t->kind == TK_OP && t->op == OP_PIPE))
    {
        if (t->kind == TK_WORD)
        {
            count++;
            t = t->next;
            continue;
        }
        if (t->kind == TK_OP && ms_is_redir_or_heredoc(t->op))
        {
            t = t->next;
            if (t)
                t = t->next;
            continue;
        }
        t = t->next;
    }
    return (count);
}

static bool argv_fill_fail(t_cmdnode *cmd, int filled)
{
    int i;

    i = 0;
    while (i < filled)
    {
        free(cmd->argv[i]);
        i++;
    }
    free(cmd->argv);
    cmd->argv = NULL;
    return (false);
}

static t_token *skip_redir_token(t_token *t)
{
    t = t->next;
    if (t)
        t = t->next;
    return (t);
}

bool ms_fill_argv(t_cmdnode *cmd, t_token *t)
{
    int i;
    int n;

    n = count_argv_words(t);
    cmd->argv = malloc(sizeof(char *) * (n + 1));
    if (!cmd->argv)
        return (false);
    i = 0;
    while (t && !(t->kind == TK_OP && t->op == OP_PIPE))
    {
        if (t->kind == TK_WORD)
        {
            cmd->argv[i] = ft_strdup(t->lex);
            if (!cmd->argv[i])
                return (argv_fill_fail(cmd, i));
            i++;
        }
        if (t->kind == TK_OP && ms_is_redir_or_heredoc(t->op))
            t = skip_redir_token(t);
        else
            t = t->next;
    }
    cmd->argv[i] = NULL;
    return (true);
}
