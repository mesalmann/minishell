/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main_utils.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hdere <hdere@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/08 12:00:00 by hdere             #+#    #+#             */
/*   Updated: 2026/03/26 21:12:18 by hdere            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static t_token	*prepare_tokens(t_ctx *ctx, char *line)
{
	t_token	*tokens;

	add_history(line);
	tokens = ms_tokenize(line, ctx);
	if (!tokens)
		return (NULL);
	ctx->cur_tokens = tokens;
	if (!ms_syntax_validate(tokens, ctx))
	{
		ms_token_free(tokens);
		ctx->cur_tokens = NULL;
		return (NULL);
	}
	if (!ms_expand_tokens(&tokens, ctx))
	{
		ctx->last_status = 1;
		ms_token_free(tokens);
		ctx->cur_tokens = NULL;
		return (NULL);
	}
	return (tokens);
}

void	ms_process_line(t_ctx *ctx, char *line)
{
	t_token		*tokens;
	t_cmdnode	*ast;

	tokens = prepare_tokens(ctx, line);
	ctx->cur_tokens = tokens;
	if (!tokens)
	{
		if (!line || !*line)
			ctx->last_status = 0;
		return ;
	}
	ast = ms_parse(tokens, ctx);
	ctx->cur_ast = ast;
	if (ast)
	{
		ms_execute_pipeline(ctx, ast);
		ms_cmd_free_list(ast);
	}
	ctx->cur_ast = NULL;
	ms_token_free(tokens);
	ctx->cur_tokens = NULL;
}
