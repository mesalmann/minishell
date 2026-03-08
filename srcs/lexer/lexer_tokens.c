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

static char	*strip_delim_quotes(const char *s)
{
	char	*out;
	int		i;
	int		k;

	if (!s)
		return (NULL);
	out = malloc(ft_strlen(s) + 1);
	if (!out)
		return (NULL);
	i = 0;
	k = 0;
	while (s[i])
	{
		if (s[i] != '\'' && s[i] != '"')
			out[k++] = s[i];
		i++;
	}
	out[k] = '\0';
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
