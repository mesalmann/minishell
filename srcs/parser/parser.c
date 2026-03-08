/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hdere <hdere@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/08 12:00:00 by hdere             #+#    #+#             */
/*   Updated: 2026/03/08 07:49:33 by hdere            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static t_token *skip_to_pipe(t_token *toks)
{
    while (toks && !(toks->kind == TK_OP && toks->op == OP_PIPE))
        toks = toks->next;
    if (toks && toks->op == OP_PIPE)
        toks = toks->next;
    return (toks);
}

static t_cmdnode *parse_error(t_cmdnode *cmd, t_cmdnode *head)
{
    ms_cmd_free(cmd);
    ms_cmd_free_list(head);
    return (NULL);
}

t_cmdnode *ms_parse(t_token *toks, t_ctx *ctx)
{
    t_cmdnode *head;
    t_cmdnode *tail;
    t_cmdnode *cmd;

    (void)ctx;
    if (!toks)
        return (NULL);
    head = NULL;
    tail = NULL;
    while (toks)
    {
        cmd = ms_cmd_new();
        if (!cmd || !ms_fill_argv(cmd, toks) || !ms_parse_ios(cmd, toks))
            return (parse_error(cmd, head));
        if (!head)
            head = cmd;
        else
            tail->next = cmd;
        tail = cmd;
        toks = skip_to_pipe(toks);
    }
    return (head);
}
