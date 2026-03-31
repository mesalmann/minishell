/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expander_internal.h                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hdere <hdere@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/12 12:00:00 by hdere             #+#    #+#             */
/*   Updated: 2026/03/12 12:00:00 by hdere            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef EXPANDER_INTERNAL_H
#define EXPANDER_INTERNAL_H

#include "minishell.h"

bool	ms_sb_grow(char **buf, size_t *cap, size_t need);
bool	ms_sb_pushc(char **buf, size_t *len, size_t *cap, char c);
bool	ms_sb_pushs(char **buf, size_t *len, size_t *cap, const char *s);
bool	ms_is_var_start(char c);
bool	ms_is_var_char(char c);
bool	is_heredoc_op(t_token *t);
void	remove_token(t_token **head, t_token *target);
bool	has_quotes(const char *s);
bool	contains_ifs(const char *s);
bool	is_redir_op(t_token *t);
void	apply_word_split(t_token **toks, t_token **p_prv,
			t_token *cur, char *exp);

#endif
