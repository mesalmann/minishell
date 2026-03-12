/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ms_export_utils.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hdere <hdere@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/08 12:00:00 by hdere             #+#    #+#             */
/*   Updated: 2026/03/08 07:37:15 by hdere            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static int  env_count(t_ctx *ctx)
{
    t_envnode   *node;
    int         n;

    n = 0;
    node = ctx->env;
    while (node)
    {
        n++;
        node = node->next;
    }
    return (n);
}

static t_envnode **env_to_arr(t_ctx *ctx, int count)
{
    t_envnode **arr;
    t_envnode *node;
    int i;

    arr = malloc(sizeof(t_envnode *) * count);
    if (!arr)
        return (NULL);
    node = ctx->env;
    i = 0;
    while (node)
    {
        arr[i++] = node;
        node = node->next;
    }
    return (arr);
}

static void sort_env_arr(t_envnode **arr, int count)
{
    int i;
    int j;
    t_envnode *tmp;

    i = 0;
    while (i < count - 1)
    {
        j = 0;
        while (j < count - 1 - i)
        {
            if (strcmp(arr[j]->key, arr[j + 1]->key) > 0)
            {
                tmp = arr[j];
                arr[j] = arr[j + 1];
                arr[j + 1] = tmp;
            }
            j++;
        }
        i++;
    }
}

static void print_export_node(t_envnode *node)
{
    ft_putstr_fd("declare -x ", STDOUT_FILENO);
    ft_putstr_fd(node->key, STDOUT_FILENO);
    if (node->has_val)
    {
        ft_putstr_fd("=\"", STDOUT_FILENO);
        if (node->val)
            ft_putstr_fd(node->val, STDOUT_FILENO);
        ft_putstr_fd("\"", STDOUT_FILENO);
    }
    ft_putstr_fd("\n", STDOUT_FILENO);
}

void print_export(t_ctx *ctx)
{
    int count;
    t_envnode **arr;
    int i;

    count = env_count(ctx);
    if (count == 0)
        return;
    arr = env_to_arr(ctx, count);
    if (!arr)
        return;
    sort_env_arr(arr, count);
    i = 0;
    while (i < count)
    {
        print_export_node(arr[i]);
        i++;
    }
    free(arr);
}
