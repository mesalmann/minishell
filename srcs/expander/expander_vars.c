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

static bool	contains_ifs(const char *s)
{
	while (s && *s)
	{
		if (*s == ' ' || *s == '\t' || *s == '\n')
			return (true);
		s++;
	}
	return (false);
}

static t_token	*make_split_token(const char *s, int start, int end)
{
	char	*lex;
	t_token	*tok;

	lex = ft_substr(s, (unsigned int)start, (size_t)(end - start));
	if (!lex)
		return (NULL);
	tok = token_new(TK_WORD, OP_NONE, lex);
	if (!tok)
	{
		free(lex);
		return (NULL);
	}
	return (tok);
}

static t_token	*split_expanded(const char *s)
{
	t_token	*head;
	t_token	*tail;
	t_token	*new;
	int		i;
	int		start;

	head = NULL;
	tail = NULL;
	i = 0;
	while (s[i])
	{
		while (s[i] && (s[i] == ' ' || s[i] == '\t' || s[i] == '\n'))
			i++;
		if (!s[i])
			break ;
		start = i;
		while (s[i] && s[i] != ' ' && s[i] != '\t' && s[i] != '\n')
			i++;
		new = make_split_token(s, start, i);
		if (!new)
		{
			ms_token_free(head);
			return (NULL);
		}
		if (!head)
			head = new;
		else
			tail->next = new;
		tail = new;
	}
	return (head);
}

static void	apply_word_split(t_token **tokens, t_token **p_prev,
					t_token *curr, char *expanded)
{
	t_token	*head;
	t_token	*tail;
	t_token	*next_saved;

	next_saved = curr->next;
	head = split_expanded(expanded);
	free(expanded);
	if (head)
	{
		tail = head;
		while (tail->next)
			tail = tail->next;
		tail->next = next_saved;
		if (*p_prev)
			(*p_prev)->next = head;
		else
			*tokens = head;
		*p_prev = tail;
	}
	else
	{
		if (*p_prev)
			(*p_prev)->next = next_saved;
		else
			*tokens = next_saved;
bool	ms_expand_tokens(t_token **tokens, t_ctx *ctx)
{
	t_token	*prev;
	t_token	*curr;
	t_token	*next;
	char	*expanded;
	bool	had_quotes;

	if (!tokens || !ctx)
		return (false);
	prev = NULL;
	curr = *tokens;
	while (curr)
	{
		next = curr->next;
		if (curr->kind == TK_WORD && curr->lex && !curr->no_expand)
		{
			had_quotes = has_quotes(curr->lex);
			expanded = ms_expand_str(ctx, curr->lex, false);
			if (!expanded)
				return (false);
			if (expanded[0] == '\0' && !had_quotes)
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
			if (!had_quotes && contains_ifs(expanded))
			{
				if (is_redir_op(prev))
					return (ambiguous_redir_err(ctx, curr, expanded));
				apply_word_split(tokens, &prev, curr, expanded);
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
