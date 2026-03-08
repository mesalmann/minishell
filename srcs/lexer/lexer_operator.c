/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer_operator.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hdere <hdere@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/08 16:30:00 by hdere             #+#    #+#             */
/*   Updated: 2026/03/08 12:00:00 by hdere            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "lexer_internal.h"

bool	ms_is_operator_char(char c)
{
	return (c == '<' || c == '>' || c == '|');
}

static t_opkind	operator_kind(const char *line, int *idx)
{
	if (line[*idx] == '<' && line[*idx + 1] == '<')
	{
		*idx += 2;
		return (OP_HEREDOC);
	}
	if (line[*idx] == '>' && line[*idx + 1] == '>')
	{
		*idx += 2;
		return (OP_OUT_APPEND);
	}
	if (line[*idx] == '<')
	{
		(*idx)++;
		return (OP_IN);
	}
	if (line[*idx] == '>')
	{
		(*idx)++;
		return (OP_OUT_TRUNC);
	}
	if (line[*idx] == '|')
	{
		(*idx)++;
		return (OP_PIPE);
	}
	return (OP_NONE);
}

t_token	*ms_handle_operator(const char *line, int *idx)
{
	t_opkind	op;

	op = operator_kind(line, idx);
	if (op == OP_NONE)
		return (NULL);
	return (token_new(TK_OP, op, NULL));
}
