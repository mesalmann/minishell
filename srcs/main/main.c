/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hdere <hdere@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/08 12:00:00 by hdere             #+#    #+#             */
/*   Updated: 2026/03/08 07:46:13 by hdere            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

volatile sig_atomic_t	g_sig = 0;

static int	ms_event_hook(void)
{
	if (g_sig == SIGINT)
		rl_done = 1;
	return (0);
}

static int	handle_input_line(t_ctx *ctx, char *line)
{
	if (g_sig == SIGINT)
	{
		ctx->last_status = 130;
		g_sig = 0;
		free(line);
		return (1);
	}
	if (line == NULL)
	{
		if (ctx->interactive)
			write(STDERR_FILENO, "exit\n", 5);
		return (0);
	}
	if (line[0] == '\0')
	{
		free(line);
		return (1);
	}
	ms_process_line(ctx, line);
	free(line);
	if (ctx->exit_requested)
		return (0);
	return (1);
}

void	ms_loop(t_ctx *ctx)
{
	char	*line;

	rl_event_hook = ms_event_hook;
	while (1)
	{
		line = readline("minishell$ ");
		if (!handle_input_line(ctx, line))
			break ;
	}
}

int	main(int ac, char **av, char **envp)
{
	t_ctx	ctx;

	(void)ac;
	(void)av;
	ms_sig_install_interactive();
	if (ms_ctx_init(&ctx, envp) == false)
		return (1);
	ms_loop(&ctx);
	ms_ctx_destroy(&ctx);
	rl_clear_history();
	return (ctx.last_status);
}
