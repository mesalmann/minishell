/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expander_sb.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hdere <hdere@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/08 12:00:00 by hdere             #+#    #+#             */
/*   Updated: 2026/03/08 07:43:59 by hdere            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

bool ms_sb_grow(char **buf, size_t *cap, size_t need)
{
    size_t newcap;
    char *nbuf;

    if (*cap >= need)
        return (true);
    newcap = *cap;
    if (newcap == 0)
        newcap = 32;
    while (newcap < need)
        newcap *= 2;
    nbuf = malloc(newcap);
    if (!nbuf)
        return (false);
    if (*buf)
    {
        memcpy(nbuf, *buf, ft_strlen(*buf) + 1);
        free(*buf);
    }
    *buf = nbuf;
    *cap = newcap;
    return (true);
}

bool ms_sb_pushc(char **buf, size_t *len, size_t *cap, char c)
{
    if (!ms_sb_grow(buf, cap, *len + 2))
        return (false);
    (*buf)[*len] = c;
    (*len)++;
    (*buf)[*len] = '\0';
    return (true);
}

bool ms_sb_pushs(char **buf, size_t *len, size_t *cap, const char *s)
{
    size_t i;

    if (!s)
        return (true);
    i = 0;
    while (s[i])
    {
        if (!ms_sb_pushc(buf, len, cap, s[i]))
            return (false);
        i++;
    }
    return (true);
}

bool ms_is_var_start(char c)
{
    return ((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') || c == '_');
}

bool ms_is_var_char(char c)
{
    return (ms_is_var_start(c) || (c >= '0' && c <= '9'));
}
