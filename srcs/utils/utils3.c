/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils3.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hdere <hdere@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/08 12:00:00 by hdere             #+#    #+#             */
/*   Updated: 2026/03/08 07:12:09 by hdere            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	ft_putendl_fd(const char *s, int fd)
{
	ft_putstr_fd(s, fd);
	write(fd, "\n", 1);
}

char	*ms_env_get(t_ctx *ctx, const char *key)
{
	t_envnode	*curr;

	if (!ctx || !ctx->env || !key)
		return (NULL);
	curr = ctx->env;
	while (curr)
	{
		if (ft_strncmp(curr->key, key, ft_strlen(key)) == 0
			&& curr->key[ft_strlen(key)] == '\0')
		{
			if (curr->has_val)
				return (curr->val);
			return (NULL);
		}
		curr = curr->next;
	}
	return (NULL);
}

static int	count_digits(long n)
{
	int	count;

	count = 0;
	if (n <= 0)
		count++;
	while (n)
	{
		n /= 10;
		count++;
	}
	return (count);
}

char	*ft_itoa(int n)
{
	long	num;
	int		len;
	char	*res;

	num = n;
	len = count_digits(num);
	res = malloc(sizeof(char) * (len + 1));
	if (!res)
		return (NULL);
	res[len] = '\0';
	if (num == 0)
		res[0] = '0';
	if (num < 0)
	{
		res[0] = '-';
		num = -num;
	}
	while (num > 0)
	{
		res[--len] = (num % 10) + '0';
		num /= 10;
	}
	return (res);
}
