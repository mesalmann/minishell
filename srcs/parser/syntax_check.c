/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   syntax_check.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hdere <hdere@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/18 17:01:28 by mesalman          #+#    #+#             */
/*   Updated: 2026/03/08 07:11:25 by hdere            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static const char	*tok_name(t_token *tok)
{
	if (!tok)
		return ("newline");
	if (tok->kind == TK_OP)
	{
		if (tok->op == OP_PIPE)
			return ("|");
		if (tok->op == OP_IN)
			return ("<");
		if (tok->op == OP_OUT_TRUNC)
			return (">");
		if (tok->op == OP_OUT_APPEND)
			return (">>");
		if (tok->op == OP_HEREDOC)
			return ("<<");
	}
	return ("word");
}

static bool	syntax_err(t_ctx *ctx, t_token *near_tok)
{
	ft_putstr_fd("minishell: syntax error near unexpected token `", 2);
	ft_putstr_fd(tok_name(near_tok), 2);
	ft_putendl_fd("'", 2);
	if (ctx)
		ctx->last_status = 2;
	return (false);
}

bool	ms_syntax_validate(t_token *tokens, t_ctx *ctx)
{
	t_token	*curr;

	if (!tokens)
		return (true);
	if (tokens->kind == TK_OP && tokens->op == OP_PIPE)
		return (syntax_err(ctx, tokens));
	curr = tokens;
	while (curr)
	{
		if (curr->kind != TK_OP)
		{
			curr = curr->next;
			continue ;
		}
		if (!curr->next)
			return (syntax_err(ctx, NULL));
		if (curr->next->kind == TK_OP)
			return (syntax_err(ctx, curr->next));
		curr = curr->next;
	}
	return (true);
}

int	ms_err_syntax(t_ctx *ctx, const char *msg)
{
	ft_putstr_fd("minishell: ", 2);
	ft_putendl_fd(msg, 2);
	if (ctx)
		ctx->last_status = 2;
	return (2);
}

int	ms_err_errno(t_ctx *ctx, const char *where)
{
	ft_putstr_fd("minishell: ", 2);
	ft_putstr_fd(where, 2);
	ft_putstr_fd(": ", 2);
	ft_putendl_fd(strerror(errno), 2);
	if (ctx)
		ctx->last_status = 1;
	return (1);
}
