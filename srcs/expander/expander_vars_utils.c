/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expander_vars_utils.c                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hdere <hdere@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/08 12:00:00 by hdere             #+#    #+#             */
/*   Updated: 2026/03/26 20:50:18 by hdere            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	remove_token(t_token **head, t_token *target)
{
	t_token	*tmp;

	if (!head || !*head || !target)
		return ;
	if (*head == target)
		*head = target->next;
	else
	{
		tmp = *head;
		while (tmp && tmp->next != target)
			tmp = tmp->next;
		if (!tmp)
			return ;
		tmp->next = target->next;
	}
	free(target->lex);
	free(target);
}

bool	has_quotes(const char *s)
{
	while (s && *s)
	{
		if (*s == '\'' || *s == '"')
			return (true);
		s++;
	}
	return (false);
}

bool	is_redir_op(t_token *t)
{
	return (t && t->kind == TK_OP
		&& (t->op == OP_IN || t->op == OP_OUT_TRUNC
			|| t->op == OP_OUT_APPEND));
}

bool	is_heredoc_op(t_token *t)
{
	return (t && t->kind == TK_OP && t->op == OP_HEREDOC);
}

bool	contains_ifs(const char *s)
{
	while (s && *s)
	{
		if (*s == ' ' || *s == '\t' || *s == '\n')
			return (true);
		s++;
	}
	return (false);
}
