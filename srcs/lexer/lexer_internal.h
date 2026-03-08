/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer_internal.h                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hdere <hdere@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/08 16:30:00 by hdere             #+#    #+#             */
/*   Updated: 2026/03/08 07:09:53 by hdere            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef LEXER_INTERNAL_H
# define LEXER_INTERNAL_H

# include "minishell.h"

bool	ms_is_operator_char(char c);
t_token	*ms_handle_operator(const char *line, int *idx);
void	ms_lex_skip_spaces(const char *line, int *idx);
t_token	*ms_handle_word(const char *line, int *idx);
int		ms_has_unmatched_quote(const char *line, int idx);
void	ms_mark_heredoc_delim(t_token *prev_op, t_token *word_tok);

#endif
