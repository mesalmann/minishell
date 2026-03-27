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
#include "expander_internal.h"

bool	ambiguous_redir_err(t_ctx *ctx, t_token *tok, char *exp)
{
	ft_putstr_fd("minishell: ", 2);
	ft_putstr_fd(tok->lex, 2);
	ft_putstr_fd(": ambiguous redirect\n", 2);
	free(exp);
	ctx->last_status = 1;
	return (false);
}

static bool	handle_empty_expansion(t_token **toks, t_token **prev,
	t_token **curr, char *exp)
{
	t_token	*next;

	next = (*curr)->next;
	if (is_heredoc_op(*prev))
	{
		free((*curr)->lex);
		(*curr)->lex = exp;
		*prev = *curr;
		*curr = next;
		return (true);
	}
	free(exp);
	remove_token(toks, *curr);
	*curr = next;
	return (true);
}

static bool	handle_word_expansion(t_token **toks, t_ctx *ctx,
	t_token **prev, t_token **curr)
{
	char		*exp;
	bool		hq;
	t_token		*next;

	next = (*curr)->next;
	hq = has_quotes((*curr)->lex);
	exp = ms_expand_str(ctx, (*curr)->lex, false);
	if (!exp)
		return (false);
	if (!hq && (exp[0] == '\0' || contains_ifs(exp)))
	{
		if (is_redir_op(*prev))
			return (ambiguous_redir_err(ctx, *curr, exp));
		if (exp[0] == '\0')
			return (handle_empty_expansion(toks, prev, curr, exp));
		apply_word_split(toks, prev, *curr, exp);
		*curr = next;
		return (true);
	}
	free((*curr)->lex);
	(*curr)->lex = exp;
	*prev = *curr;
	*curr = next;
	return (true);
}

bool	ms_expand_tokens(t_token **tokens, t_ctx *ctx)
{
	t_token	*prev;
	t_token	*curr;

	if (!tokens || !ctx)
		return (false);
	prev = NULL;
	curr = *tokens;
	while (curr)
	{
		if (curr->kind == TK_WORD && curr->lex && !curr->no_expand)
		{
			if (!handle_word_expansion(tokens, ctx, &prev, &curr))
				return (false);
		}
		else
		{
			prev = curr;
			curr = curr->next;
		}
	}
	return (true);
}
