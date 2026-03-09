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

static void	append_token(t_token **head, t_token **tail, t_token *new)
{
	if (!*head)
		*head = new;
	else
		(*tail)->next = new;
	*tail = new;
}

static bool	report_unmatched_quote(t_ctx *ctx, t_token **head, int quote)
{
	char	c;

	c = (char)quote;
	ms_token_free(*head);
	ft_putstr_fd("minishell: unexpected EOF while looking", 2);
	ft_putstr_fd(" for matching `", 2);
	write(2, &c, 1);
	ft_putendl_fd("'", 2);
	if (ctx)
		ctx->last_status = 2;
	return (false);
}

static bool	report_syntax_error(t_ctx *ctx, t_token **head)
{
	ms_token_free(*head);
	ft_putendl_fd("minishell: syntax error near unexpected token", 2);
	if (ctx)
		ctx->last_status = 2;
	return (false);
}

static bool	build_token(t_ctx *ctx, const char *line, t_token **lst, int *idx)
{
	t_token	*new;
	int		uq;

	if (ms_is_operator_char(line[*idx]))
		new = ms_handle_operator(line, idx);
	else
	{
		uq = ms_has_unmatched_quote(line, *idx);
		new = ms_handle_word(line, idx);
		if (!new && uq)
			return (report_unmatched_quote(ctx, &lst[0], uq));
	}
	if (!new)
		return (report_syntax_error(ctx, &lst[0]));
	if (new->kind == TK_WORD && lst[1] && lst[1]->op == OP_HEREDOC)
		ms_mark_heredoc_delim(lst[1], new);
	append_token(&lst[0], &lst[1], new);
	return (true);
}

t_token	*ms_tokenize(const char *line, t_ctx *ctx)
{
	t_token	*lst[2];
	int		idx;

	lst[0] = NULL;
	lst[1] = NULL;
	idx = 0;
	while (line && line[idx])
	{
		ms_lex_skip_spaces(line, &idx);
		if (!line[idx])
			break ;
		if (!build_token(ctx, line, lst, &idx))
			return (NULL);
	}
	return (lst[0]);
}
