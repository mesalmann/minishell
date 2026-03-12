/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_internal.h                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hdere <hdere@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/12 12:00:00 by hdere             #+#    #+#             */
/*   Updated: 2026/03/12 12:00:00 by hdere            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PARSER_INTERNAL_H
# define PARSER_INTERNAL_H

# include "minishell.h"

t_cmdnode	*ms_cmd_new(void);
t_redir		*ms_redir_new(t_redirtype type, char *target);
t_heredoc	*ms_heredoc_new(char *delim, bool expand_mode);
void		ms_redir_add_back(t_redir **lst, t_redir *new);
void		ms_heredoc_add_back(t_heredoc **lst, t_heredoc *new);
bool		ms_fill_argv(t_cmdnode *cmd, t_token *t);
bool		ms_parse_ios(t_cmdnode *cmd, t_token *t);

#endif
