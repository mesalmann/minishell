/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init_utils.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hdere <hdere@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/08 12:00:00 by hdere             #+#    #+#             */
/*   Updated: 2026/03/26 21:12:21 by hdere            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static void	destroy_env_nodes(t_ctx *ctx)
{
	t_envnode	*node;
	t_envnode	*tmp;

	node = ctx->env;
	while (node)
	{
		tmp = node->next;
		free(node->key);
		free(node->val);
		free(node);
		node = tmp;
	}
	ctx->env = NULL;
}

void	ms_ctx_destroy(t_ctx *ctx)
{
	if (!ctx)
		return ;
	destroy_env_nodes(ctx);
	free_tab(ctx->envp_cache);
	ctx->envp_cache = NULL;
	if (ctx->cur_ast)
	{
		ms_cmd_free_list(ctx->cur_ast);
		ctx->cur_ast = NULL;
	}
	if (ctx->cur_tokens)
	{
		ms_token_free(ctx->cur_tokens);
		ctx->cur_tokens = NULL;
	}
}
