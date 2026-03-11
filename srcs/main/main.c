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

volatile sig_atomic_t g_sig = 0;

static void handle_sigint(int sig)
{
	(void)sig;
	write(1, "\n", 1);
	rl_on_new_line();
	g_sig = SIGINT;
}

void ms_sig_install_interactive(void)
{
	struct sigaction sa;

	sa.sa_flags = 0;
	sigemptyset(&sa.sa_mask);
	sa.sa_handler = handle_sigint;
	sigaction(SIGINT, &sa, NULL);
	sa.sa_handler = SIG_IGN;
	sigaction(SIGQUIT, &sa, NULL);
}

static void	ms_process_line(t_ctx *ctx, char *line)
{
	t_token		*tokens;
	t_cmdnode	*ast;

	add_history(line);
	tokens = ms_tokenize(line, ctx);
	if (!tokens)
		return ;
	ctx->cur_tokens = tokens;
	if (!ms_syntax_validate(tokens, ctx))
	{
		ms_token_free(tokens);
		ctx->cur_tokens = NULL;
		return ;
	}
	if (!ms_expand_tokens(&tokens, ctx))
	{
		ctx->last_status = 1;
		ms_token_free(tokens);
		ctx->cur_tokens = NULL;
		return ;
	}
	ctx->cur_tokens = tokens;
	if (!tokens)
	{
		ctx->last_status = 0;
		ctx->cur_tokens = NULL;
		return ;
	}
	ast = ms_parse(tokens, ctx);
	if (ast)
	{
		ctx->cur_ast = ast;
		ms_execute_pipeline(ctx, ast);
		ms_cmd_free_list(ast);
		ctx->cur_ast = NULL;
	}
	ms_token_free(tokens);
	ctx->cur_tokens = NULL;
}

void ms_loop(t_ctx *ctx, char **envp)
{
	char *line;

	(void)envp;
	while (1)
	{
		if (g_sig == SIGINT)
		{
			ctx->last_status = 130;
			g_sig = 0;
		}
		line = readline("minishell$ ");
		if (line == NULL)
		{
			if (ctx->interactive)
				write(STDERR_FILENO, "exit\n", 5);
			break;
		}
		if (line[0] == '\0')
		{
			free(line);
			continue;
		}
		ms_process_line(ctx, line);
		free(line);
		if (ctx->exit_requested)
			break ;
	}
}

int main(int ac, char **av, char **envp)
{
	t_ctx ctx;

	(void)ac;
	(void)av;
	ms_sig_install_interactive();
	if (ms_ctx_init(&ctx, envp) == false)
		return (1);
	ms_loop(&ctx, envp);
	ms_ctx_destroy(&ctx);
	rl_clear_history();
	return (ctx.last_status);
}
