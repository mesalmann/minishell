/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expander_vars.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hdere <hdere@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/08 12:00:00 by hdere             #+#    #+#             */
/*   Updated: 2026/03/08 07:09:48 by hdere            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

bool	ms_expand_tokens(t_token *tokens, t_ctx *ctx)
{
	t_token	*curr;
	char	*expanded;

	curr = tokens;
	while (curr)
	{
		if (curr->kind != TK_WORD || curr->lex == NULL || curr->no_expand)
		{
			curr = curr->next;
			continue ;
		}
		expanded = ms_expand_str(ctx, curr->lex, false);
		if (!expanded)
			return (false);
		free(curr->lex);
		curr->lex = expanded;
		curr = curr->next;
	}
	return (true);
}
