/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expander_core.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hdere <hdere@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/08 12:00:00 by hdere             #+#    #+#             */
/*   Updated: 2026/03/08 07:44:39 by hdere            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "expander_internal.h"

static bool expand_var_name(t_ctx *ctx, t_expbuf *eb, const char *in)
{
    int start;
    char *name;
    char *val;

    start = eb->i;
    while (in[eb->i] && ms_is_var_char(in[eb->i]))
        eb->i++;
    name = ft_substr(in, (unsigned int)start, (size_t)(eb->i - start));
    if (!name)
        return (false);
    val = ms_env_get(ctx, name);
    free(name);
    return (ms_sb_pushs(&eb->out, &eb->len, &eb->cap, val));
}

static bool expand_dollar(t_ctx *ctx, t_expbuf *eb, const char *in)
{
    char *val;

    eb->i++;
    if (in[eb->i] == '?')
    {
        val = ft_itoa(ctx->last_status);
        if (!val)
            return (false);
        if (!ms_sb_pushs(&eb->out, &eb->len, &eb->cap, val))
        {
            free(val);
            return (false);
        }
        free(val);
        eb->i++;
        return (true);
    }
    if (ms_is_var_start(in[eb->i]))
        return (expand_var_name(ctx, eb, in));
    if (in[eb->i] >= '0' && in[eb->i] <= '9')
    {
        eb->i++;
        return (true);
    }
    return (ms_sb_pushc(&eb->out, &eb->len, &eb->cap, '$'));
}

static bool handle_quote_toggle(t_expbuf *eb, char c)
{
    if (eb->st == Q_NONE && c == '\'')
        eb->st = Q_SINGLE;
    else if (eb->st == Q_NONE && c == '"')
        eb->st = Q_DOUBLE;
    else if (eb->st == Q_SINGLE && c == '\'')
        eb->st = Q_NONE;
    else if (eb->st == Q_DOUBLE && c == '"')
        eb->st = Q_NONE;
    else
        return (false);
    eb->i++;
    return (true);
}

char *ms_expand_str(t_ctx *ctx, const char *in, bool in_dquote)
{
    t_expbuf eb;

    ft_memset(&eb, 0, sizeof(t_expbuf));
    if (!ms_sb_grow(&eb.out, &eb.cap, 1))
        return (NULL);
    eb.out[0] = '\0';
    while (in && in[eb.i])
    {
        if (!in_dquote && handle_quote_toggle(&eb, in[eb.i]))
            continue;
        if (in[eb.i] == '$' && eb.st != Q_SINGLE)
        {
            if (!expand_dollar(ctx, &eb, in))
            {
                free(eb.out);
                return (NULL);
            }
            continue;
        }
        if (!ms_sb_pushc(&eb.out, &eb.len, &eb.cap, in[eb.i]))
        {
            free(eb.out);
            return (NULL);
        }
        eb.i++;
    }
    return (eb.out);
}
