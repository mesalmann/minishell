/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer_tokens.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hdere <hdere@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/08 16:30:00 by hdere             #+#    #+#             */
/*   Updated: 2026/03/08 12:00:00 by hdere            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "lexer_internal.h"

static bool	is_quoted_word(const char *s)
{
	int	i;

	if (!s)
		return (false);
	i = 0;
	while (s[i])
	{
		if (s[i] == '\'' || s[i] == '"')
			return (true);
		i++;
	}
	return (false);
}

static void	strip_copy(const char *s, char *out, int *k)
{
	t_qstate	st;
	int			i;

	st = Q_NONE;
	i = 0;
	while (s[i])
	{
		if (st == Q_NONE && s[i] == '\'')
			st = Q_SINGLE;
		else if (st == Q_NONE && s[i] == '"')
			st = Q_DOUBLE;
		else if (st == Q_SINGLE && s[i] == '\'')
			st = Q_NONE;
		else if (st == Q_DOUBLE && s[i] == '"')
			st = Q_NONE;
		else if (out)
			out[(*k)++] = s[i];
		else
			(*k)++;
		i++;
	}
}

static char	*strip_delim_quotes(const char *s)
{
	char	*out;
	int		len;

	if (!s)
		return (NULL);
	len = 0;
	strip_copy(s, NULL, &len);
	out = malloc((size_t)len + 1);
	if (!out)
		return (NULL);
	len = 0;
	strip_copy(s, out, &len);
	out[len] = '\0';
	return (out);
}

void	ms_mark_heredoc_delim(t_token *prev_op, t_token *word_tok)
{
	char	*stripped;

	if (!prev_op || prev_op->op != OP_HEREDOC || !word_tok)
		return ;
	word_tok->no_expand = is_quoted_word(word_tok->lex);
	stripped = strip_delim_quotes(word_tok->lex);
	if (!stripped)
		return ;
	free(word_tok->lex);
	word_tok->lex = stripped;
}
