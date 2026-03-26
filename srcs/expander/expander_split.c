/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expander_split.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hdere <hdere@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/08 12:00:00 by hdere             #+#    #+#             */
/*   Updated: 2026/03/26 20:53:32 by hdere            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static bool	add_split_tok(t_token **hd, t_token **tl, char *lex)
{
	t_token	*tok;

	if (!lex)
		return (false);
	tok = token_new(TK_WORD, OP_NONE, lex);
	if (!tok)
	{
		free(lex);
		return (false);
	}
	if (!*hd)
		*hd = tok;
	else
		(*tl)->next = tok;
	*tl = tok;
	return (true);
}

t_token	*split_expanded(const char *s)
{
	t_token	*head;
	t_token	*tail;
	int		i;
	int		st;

	head = NULL;
	i = 0;
	while (s[i])
	{
		while (s[i] && (s[i] == ' ' || s[i] == '\t' || s[i] == '\n'))
			i++;
		if (!s[i])
			break ;
		st = i;
		while (s[i] && s[i] != ' ' && s[i] != '\t' && s[i] != '\n')
			i++;
		if (!add_split_tok(&head, &tail,
				ft_substr(s, (unsigned int)st, (size_t)(i - st))))
		{
			ms_token_free(head);
			return (NULL);
		}
	}
	return (head);
}

static void	link_split_tokens(t_token **toks, t_token **p_prv,
					t_token *next_s, t_token *head)
{
	t_token	*tail;

	if (head)
	{
		tail = head;
		while (tail->next)
			tail = tail->next;
		tail->next = next_s;
		if (*p_prv)
			(*p_prv)->next = head;
		else
			*toks = head;
		*p_prv = tail;
	}
	else
	{
		if (*p_prv)
			(*p_prv)->next = next_s;
		else
			*toks = next_s;
	}
}

void	apply_word_split(t_token **toks, t_token **p_prv,
					t_token *cur, char *exp)
{
	t_token	*head;
	t_token	*next_s;

	next_s = cur->next;
	head = split_expanded(exp);
	free(exp);
	link_split_tokens(toks, p_prv, next_s, head);
	free(cur->lex);
	free(cur);
}
