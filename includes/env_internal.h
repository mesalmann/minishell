/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   env_internal.h                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hdere <hdere@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/26 21:00:00 by hdere             #+#    #+#             */
/*   Updated: 2026/03/26 21:36:01 by hdere            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef ENV_INTERNAL_H
#define ENV_INTERNAL_H

#include "minishell.h"
#include <signal.h>

t_envnode *node_new(const char *key, const char *val, bool has_val);
void _sigblock_env(sigset_t *new_set, sigset_t *old_set);
char *get_val_str(const char *val);

#endif
