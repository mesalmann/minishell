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
# define EXPANDER_INTERNAL_H

# include "minishell.h"

bool	ms_sb_grow(char **buf, size_t *cap, size_t need);
bool	ms_sb_pushc(char **buf, size_t *len, size_t *cap, char c);
bool	ms_sb_pushs(char **buf, size_t *len, size_t *cap, const char *s);
bool	ms_is_var_start(char c);
bool	ms_is_var_char(char c);

#endif
