/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expander_vars.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hdere <hdere@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/08 12:00:00 by hdere             #+#    #+#             */
/*   Updated: 2026/03/08 07:09:48 by hdere            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static void	remove_token(t_token **head, t_token *target)
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

static bool	has_quotes(const char *s)
{
	while (s && *s)
	{
		if (*s == '\'' || *s == '"')
			return (true);
		s++;
	}
	return (false);
}

static bool	is_redir_op(t_token *t)
{
	return (t && t->kind == TK_OP
		&& (t->op == OP_IN || t->op == OP_OUT_TRUNC
			|| t->op == OP_OUT_APPEND));
}

static bool	is_heredoc_op(t_token *t)
{
	return (t && t->kind == TK_OP && t->op == OP_HEREDOC);
}

static bool	ambiguous_redir_err(t_ctx *ctx, t_token *tok, char *exp)
{
	ft_putstr_fd("minishell: ", 2);
	ft_putstr_fd(tok->lex, 2);
	ft_putstr_fd(": ambiguous redirect\n", 2);
	free(exp);
	ctx->last_status = 1;
	return (false);
}

bool	ms_expand_tokens(t_token **tokens, t_ctx *ctx)
{
	t_token	*prev;
	t_token	*curr;
	t_token	*next;
	char	*expanded;

	if (!tokens || !ctx)
		return (false);
	prev = NULL;
	curr = *tokens;
	while (curr)
	{
		next = curr->next;
		if (curr->kind == TK_WORD && curr->lex && !curr->no_expand)
		{
			expanded = ms_expand_str(ctx, curr->lex, false);
			if (!expanded)
				return (false);
			if (expanded[0] == '\0' && !has_quotes(curr->lex))
			{
				if (is_redir_op(prev))
					return (ambiguous_redir_err(ctx, curr, expanded));
				if (is_heredoc_op(prev))
				{
					free(curr->lex);
					curr->lex = expanded;
					prev = curr;
					curr = next;
					continue ;
				}
				free(expanded);
				remove_token(tokens, curr);
				curr = next;
				continue ;
			}
			free(curr->lex);
			curr->lex = expanded;
		}
		prev = curr;
		curr = next;
	}
	return (true);
}
