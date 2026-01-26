/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_split.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mesalman <mesalman@student.42istanbul.com  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/17 13:23:08 by mesalman          #+#    #+#             */
/*   Updated: 2025/06/26 15:18:16 by mesalman         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int is_white_space(char c)
{
    if (c == ' ' || c == '\t' || c == '\f'
        || c == '\r' || c == '\n' || c == '\v')
        return (1);
    return (0);
}

int count_words(const char *s)
{
    int i = 0;
    int count = 0;

    while (s[i])
    {
        while (s[i] && is_white_space(s[i]))
            i++;
        if (s[i])
        {
            count++;
            while (s[i] && !is_white_space(s[i]))
                i++;
        }
    }
    return (count);
}

static void free_arr(char **arr, int j)
{
    while (--j >= 0)
        free(arr[j]);
    free(arr);
}

static int fill_array(char **arr, const char *s)
{
    int i = 0;
    int j = 0;
    int start = 0;

    while (s[i])
    {
        while (s[i] && is_white_space(s[i]))
            i++;
        start = i;
        while (s[i] && !is_white_space(s[i]))
            i++;
        if (i > start)
        {
            arr[j] = ft_substr(s, start, i - start);
            if (!arr[j])
                return (free_arr(arr, j), 0);
            j++;
        }
    }
    arr[j] = NULL;
    return (1);
}

char **ft_split(const char *s)
{
    char **arr;

    if (!s)
        return (NULL);

    arr = malloc((count_words(s) + 1) * sizeof(char *));
    if (!arr)
        return (NULL);
    if (!fill_array(arr, s))
        return (NULL);

    return (arr);
}
