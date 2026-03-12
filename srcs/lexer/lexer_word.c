/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer_word.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hdere <hdere@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/08 16:30:00 by hdere             #+#    #+#             */
/*   Updated: 2026/03/08 12:00:00 by hdere            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "lexer_internal.h"

void	ms_lex_skip_spaces(const char *line, int *idx)
{
	while (line && line[*idx] && isspace((unsigned char)line[*idx]))
		(*idx)++;
}

static void	update_qstate(t_qstate *state, char c)
{
	if (*state == Q_NONE && c == '\'')
		*state = Q_SINGLE;
	else if (*state == Q_NONE && c == '"')
		*state = Q_DOUBLE;
	else if (*state == Q_SINGLE && c == '\'')
		*state = Q_NONE;
	else if (*state == Q_DOUBLE && c == '"')
		*state = Q_NONE;
}

static int	word_process(const char *s, int start, char *out)
{
	t_qstate	state;
	int			len;
	int			i;
	int			k;

	state = Q_NONE;
	len = 0;
	i = start;
	k = 0;
	while (s[i])
	{
		if (state == Q_NONE
			&& (isspace((unsigned char)s[i])
				|| ms_is_operator_char(s[i])))
			break ;
		if (out)
			out[k++] = s[i];
		update_qstate(&state, s[i]);
		len++;
		i++;
	}
	if (state != Q_NONE)
		return (-1);
	if (out)
		out[k] = '\0';
	return (len);
}

static char	*dup_word(const char *line, int start, int *len_out)
{
	char	*lex;
	int		len;

	len = word_process(line, start, NULL);
	if (len < 0)
		return (NULL);
	lex = malloc((size_t)len + 1);
	if (!lex)
		return (NULL);
	if (word_process(line, start, lex) < 0)
	{
		free(lex);
		return (NULL);
	}
	if (len_out)
		*len_out = len;
	return (lex);
}

t_token	*ms_handle_word(const char *line, int *idx)
{
	t_token	*new;
	char	*lex;
	int		len;

	lex = dup_word(line, *idx, &len);
	if (!lex)
		return (NULL);
	new = token_new(TK_WORD, OP_NONE, lex);
	if (!new)
	{
		free(lex);
		return (NULL);
	}
	*idx += len;
	return (new);
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
