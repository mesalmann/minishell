/* ************************************************************************** */
/* */
/* :::      ::::::::   */
/* ms_exit.c                                          :+:      :+:    :+:   */
/* +:+ +:+         +:+     */
/* By: hdere <hdere@student.42.fr>                +#+  +:+       +#+        */
/* +#+#+#+#+#+   +#+           */
/* Created: 2026/03/09 10:00:00 by hdere             #+#    #+#             */
/* Updated: 2026/03/09 10:45:00 by hdere            ###   ########.fr       */
/* */
/* ************************************************************************** */

#include "minishell.h"
#include <limits.h>

static int	is_ws(char c)
{
	return (c == ' ' || (c >= 9 && c <= 13));
}

static int	parse_exit_code(const char *s, long long *res)
{
	int					i;
	int					neg;
	unsigned long long	cutoff;
	unsigned long long	u_res;

	i = 0;
	neg = 0;
	u_res = 0;
	while (is_ws(s[i]))
		i++;
	if (s[i] == '-' || s[i] == '+')
		if (s[i++] == '-')
			neg = 1;
	if (!(s[i] >= '0' && s[i] <= '9'))
		return (1);
	cutoff = (unsigned long long)LLONG_MAX + neg;
	while (s[i] >= '0' && s[i] <= '9')
	{
		if (u_res > (cutoff - (s[i] - '0')) / 10)
			return (1);
		u_res = u_res * 10 + (s[i++] - '0');
	}
	while (is_ws(s[i]))
		i++;
	if (s[i] != '\0')
		return (1);
	*res = (neg) ? -(long long)u_res : (long long)u_res;
	return (0);
}

static int	exit_error(char *arg, char *msg, int status)
{
	ft_putstr_fd("minishell: exit: ", STDERR_FILENO);
	if (arg)
	{
		ft_putstr_fd(arg, STDERR_FILENO);
		ft_putstr_fd(": ", STDERR_FILENO);
	}
	ft_putendl_fd(msg, STDERR_FILENO);
	return (status);
}

int	ms_builtin_exit(t_ctx *ctx, char **argv)
{
	long long	status;

	if (!argv || !argv[0])
		return (0);
	if (!argv[1])
		return (-1);
	if (parse_exit_code(argv[1], &status))
	{
		ctx->last_status = exit_error(argv[1], "numeric argument required", 255);
		return (-1);
	}
	if (argv[2])
	{
		exit_error(NULL, "too many arguments", 1);
		ctx->last_status = 1;
		return (1);
	}
	ctx->last_status = (int)(status & 0xFF);
	return (-1);
}