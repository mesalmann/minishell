/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer_utils.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hdere <hdere@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/08 16:30:00 by hdere             #+#    #+#             */
/*   Updated: 2026/03/26 21:00:31 by hdere            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "lexer_internal.h"

void	ms_lex_skip_spaces(const char *line, int *idx)
{
	while (line && line[*idx] && isspace((unsigned char)line[*idx]))
		(*idx)++;
}

int	ms_has_unmatched_quote(const char *s, int idx)
{
	t_qstate	state;

	state = Q_NONE;
	while (s[idx])
	{
		if (state == Q_NONE
			&& (isspace((unsigned char)s[idx])
				|| ms_is_operator_char(s[idx])))
			break ;
		if (state == Q_NONE && s[idx] == '\'')
			state = Q_SINGLE;
		else if (state == Q_NONE && s[idx] == '"')
			state = Q_DOUBLE;
		else if (state == Q_SINGLE && s[idx] == '\'')
			state = Q_NONE;
		else if (state == Q_DOUBLE && s[idx] == '"')
			state = Q_NONE;
		idx++;
	}
	if (state == Q_SINGLE)
		return ('\'');
	if (state == Q_DOUBLE)
		return ('"');
	return (0);
}

t_token	*token_new(t_tokkind kind, t_opkind op, char *lex)
{
	t_token	*tok;

	tok = malloc(sizeof(t_token));
	if (!tok)
		return (NULL);
	tok->kind = kind;
	tok->op = op;
	tok->lex = lex;
	tok->no_expand = false;
	tok->next = NULL;
	return (tok);
}

void	ms_token_free(t_token *toks)
{
	t_token	*tmp;

	while (toks)
	{
		tmp = toks->next;
		if (toks->lex)
			free(toks->lex);
		free(toks);
		toks = tmp;
	}
}
