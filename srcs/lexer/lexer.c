/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hdere <hdere@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/08 16:30:00 by hdere             #+#    #+#             */
/*   Updated: 2026/03/08 06:27:44 by hdere            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "lexer_internal.h"

static void	mark_heredoc_if_needed(t_token *prev, t_token *new)
{
	if (!prev || prev->kind != TK_OP || prev->op != OP_HEREDOC)
		return ;
	if (!new || new->kind != TK_WORD)
		return ;
	ms_mark_heredoc_delim(prev, new);
}

static bool	append_token(t_token **head, t_token **tail, t_token *new)
{
	if (!new)
		return (false);
	if (!*head)
	{
		*head = new;
		*tail = new;
		return (true);
	}
	mark_heredoc_if_needed(*tail, new);
	(*tail)->next = new;
	*tail = new;
	return (true);
}

static t_token	*lex_one_token(const char *line, int *idx)
{
	if (ms_is_operator_char(line[*idx]))
		return (ms_handle_operator(line, idx));
	return (ms_handle_word(line, idx));
}

t_token	*ms_tokenize(const char *line, t_ctx *ctx)
{
	t_token	*head;
	t_token	*tail;
	t_token	*new;
	int		idx;

	(void)ctx;
	if (!line)
		return (NULL);
	head = NULL;
	tail = NULL;
	idx = 0;
	while (line[idx])
	{
		ms_lex_skip_spaces(line, &idx);
		if (!line[idx])
			break ;
		new = lex_one_token(line, &idx);
		if (!append_token(&head, &tail, new))
		{
			ms_token_free(head);
			return (NULL);
		}
	}
	return (head);
}
