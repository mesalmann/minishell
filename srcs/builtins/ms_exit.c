/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ms_exit.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hdere <hdere@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/08 12:00:00 by hdere             #+#    #+#             */
/*   Updated: 2026/03/08 07:32:08 by hdere            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../minishell.h"
#include <limits.h>

static int is_numeric(const char *s)
{
	int i;

	i = 0;
	if (s[i] == '-' || s[i] == '+')
		i++;
	if (!s[i])
		return (0);
	while (s[i])
	{
		if (s[i] < '0' || s[i] > '9')
			return (0);
		i++;
	}
	return (1);
}

static int is_overflow(const char *s)
{
	int i;
	int neg;
	unsigned long long res;
	unsigned long long limit;

	i = 0;
	neg = 0;
	res = 0;
	if (s[i] == '-' || s[i] == '+')
	{
		neg = (s[i] == '-');
		i++;
	}
	limit = (unsigned long long)LLONG_MAX + neg;
	while (s[i])
	{
		if (res > (limit - (s[i] - '0')) / 10)
			return (1);
		res = res * 10 + (s[i] - '0');
		if (res > limit)
			return (1);
		i++;
	}
	return (0);
}

static long long ft_atoll(const char *s)
{
	long long res;
	int sign;
	int i;

	res = 0;
	sign = 1;
	i = 0;
	if (s[i] == '-' || s[i] == '+')
	{
		if (s[i] == '-')
			sign = -1;
		i++;
	}
	while (s[i] >= '0' && s[i] <= '9')
	{
		res = res * 10 + (s[i] - '0');
		i++;
	}
	return (res * sign);
}

static void exit_numeric_error(t_ctx *ctx, const char *arg)
{
	ft_putstr_fd("minishell: exit: ", STDERR_FILENO);
	ft_putstr_fd(arg, STDERR_FILENO);
	ft_putendl_fd(": numeric argument required", STDERR_FILENO);
	ms_ctx_destroy(ctx);
	exit(2);
}

int ms_builtin_exit(t_ctx *ctx, char **argv)
{
	int status;

	ft_putendl_fd("exit", STDERR_FILENO);
	if (!argv[1])
	{
		status = ctx->last_status;
		ms_ctx_destroy(ctx);
		exit(status);
	}
	if (!is_numeric(argv[1]) || is_overflow(argv[1]))
		exit_numeric_error(ctx, argv[1]);
	if (argv[2])
	{
		ft_putendl_fd("minishell: exit: too many arguments", STDERR_FILENO);
		return (1);
	}
	status = (int)(ft_atoll(argv[1]) & 0xFF);
	ms_ctx_destroy(ctx);
	exit(status);
}
