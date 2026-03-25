/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   signals.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hdere <hdere@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/12 12:00:00 by hdere             #+#    #+#             */
/*   Updated: 2026/03/12 12:00:00 by hdere            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/*
** Interactive prompt handler: readline aktifken SIGINT gelince
** mevcut satiri temizleyip yeni prompt basar.
*/
static void	handle_sigint_interactive(int sig)
{
	(void)sig;
	g_sig = SIGINT;
	write(1, "\n", 1);
}

/*
** Heredoc handler: Sinyal flag'i set eder.
** POSIX-safe: close(STDIN) yerine flag kullanilir.
** ms_run_heredocs'te check yapilir ve readline loop iptal edilir.
*/
static void	handle_sigint_heredoc(int sig)
{
	(void)sig;
	g_sig = SIGINT;
	write(1, "\n", 1);
}

void	ms_sig_install_interactive(void)
{
	struct sigaction	sa;

	sa.sa_flags = 0;
	sigemptyset(&sa.sa_mask);
	sa.sa_handler = handle_sigint_interactive;
	sigaction(SIGINT, &sa, NULL);
	sa.sa_handler = SIG_IGN;
	sigaction(SIGQUIT, &sa, NULL);

}

/*
** Exec mod: fork sonrasi parent beklerken kullanilir.
** SIGINT -> SIG_IGN (parent sessiz kalir, child olur)
** SIGQUIT -> SIG_IGN (parent sessiz kalir)
*/
void	ms_sig_install_exec(void)
{
	struct sigaction	sa;

	sa.sa_flags = 0;
	sigemptyset(&sa.sa_mask);
	sa.sa_handler = SIG_IGN;
	sigaction(SIGINT, &sa, NULL);
	sigaction(SIGQUIT, &sa, NULL);
}

/*
** Heredoc mod: heredoc readline sirasinda kullanilir.
** SIGINT -> handler_heredoc (flag + stdin kapat)
** SIGQUIT -> SIG_IGN
*/
void	ms_sig_install_heredoc(void)
{
	struct sigaction	sa;

	sa.sa_flags = 0;
	sigemptyset(&sa.sa_mask);
	sa.sa_handler = handle_sigint_heredoc;
	sigaction(SIGINT, &sa, NULL);
	sa.sa_handler = SIG_IGN;
	sigaction(SIGQUIT, &sa, NULL);
}

/*
** Child process reset: execve oncesi tum signallari default'a dondurur.
*/
void	ms_sig_child_reset(void)
{
	signal(SIGINT, SIG_DFL);
	signal(SIGQUIT, SIG_DFL);
}

/*
** waitpid sonrasi signal kaynakli olum mesajlarini basar.
** Bash: SIGQUIT -> "Quit (core dumped)\n"
*/
void	ms_print_signal_msg(int status)
{
	if (WIFSIGNALED(status))
	{
		if (WTERMSIG(status) == SIGQUIT)
			ft_putstr_fd("Quit (core dumped)\n", STDERR_FILENO);
		else if (WTERMSIG(status) == SIGINT)
			write(STDERR_FILENO, "\n", 1);
	}
}
