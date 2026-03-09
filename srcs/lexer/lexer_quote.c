/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer_quote.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hdere <hdere@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/08 16:30:00 by hdere             #+#    #+#             */
/*   Updated: 2026/03/08 12:00:00 by hdere            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "lexer_internal.h"

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
