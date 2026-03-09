/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ms_echo.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hdere <hdere@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/08 12:00:00 by hdere             #+#    #+#             */
/*   Updated: 2026/03/08 06:41:47 by hdere            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../minishell.h"

/*
** is_n_flag - argüman geçerli bir -n bayrağı mı?
** Geçerli form: '-' ve ardından en az bir 'n' (sadece 'n').
** Örnekler: -n, -nn, -nnn → geçerli.
**            -na, -x     → geçersiz.
*/
static int	is_n_flag(const char *s)
{
	int	i;

	if (!s || s[0] != '-' || !s[1])
		return (0);
	i = 1;
	while (s[i])
	{
		if (s[i] != 'n')
			return (0);
		i++;
	}
	return (1);
}

/*
** ms_builtin_echo - echo [-n] [arg ...]
**
** Kural:
**   - Başındaki ardışık -n/-nn/... argumanları bayrak olarak yenir.
**   - Kalan argümanlar boşlukla ayrılarak stdout'a yazılır.
**   - -n yoksa sonda '\n' basılır; varsa basılmaz.
**   - Her zaman 0 döner.
*/
int	ms_builtin_echo(char **argv)
{
	int	no_newline;
	int	i;

	no_newline = 0;
	i = 1;
	while (argv[i] && is_n_flag(argv[i]))
	{
		no_newline = 1;
		i++;
	}
	while (argv[i])
	{
		ft_putstr_fd(argv[i], STDOUT_FILENO);
		if (argv[i + 1])
			ft_putstr_fd(" ", STDOUT_FILENO);
		i++;
	}
	if (!no_newline)
		ft_putstr_fd("\n", STDOUT_FILENO);
	return (0);
}
