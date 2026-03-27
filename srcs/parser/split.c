/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   split.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hdere <hdere@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/08 12:00:00 by hdere             #+#    #+#             */
/*   Updated: 2026/03/08 07:11:21 by hdere            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

#include "minishell.h"

static int	is_sep(char s, char c)
{
	if (s == c)
		return (1);
	return (0);
}

static int	count_words(char const *s, char c)
{
	int	i;
	int	count;

	i = 0;
	count = 0;
	while (s[i])
	{
		while (s[i] && is_sep(s[i], c))
			i++;
		if (s[i])
		{
			count++;
			while (s[i] && !is_sep(s[i], c))
				i++;
		}
	}
	return (count);
}

static char	*word_dup(const char *str, int start, int finish)
{
	char	*word;
	int		i;

	word = malloc((finish - start + 1) * sizeof(char));
	if (!word)
		return (NULL);
	i = 0;
	while (start < finish)
		word[i++] = str[start++];
	word[i] = '\0';
	return (word);
}

static char	**fill_split(char **split, char const *s, char c)
{
	size_t	i;
	size_t	j;
	int		idx;

	i = -1;
	j = 0;
	idx = -1;
	while (++i <= ft_strlen(s))
	{
		if (s[i] != c && idx < 0)
			idx = i;
		else if ((s[i] == c || s[i] == '\0') && idx >= 0)
		{
			split[j] = word_dup(s, idx, i);
			if (!split[j])
			{
				free_tab(split);
				return (NULL);
			}
			j++;
			idx = -1;
		}
	}
	split[j] = NULL;
	return (split);
}

char	**ft_split(char const *s, char c)
{
	char	**split;

	if (!s)
		return (NULL);
	split = malloc((count_words(s, c) + 1) * sizeof(char *));
	if (!split)
		return (NULL);
	return (fill_split(split, s, c));
}
