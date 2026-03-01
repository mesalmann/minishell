#include "../../minishell.h"
#include <string.h>

int	ms_is_builtin_argv(char **argv)
{
	if (!argv || !argv[0])
		return (0);
	if (strcmp(argv[0], "echo") == 0)
		return (1);
	if (strcmp(argv[0], "pwd") == 0)
		return (1);
	if (strcmp(argv[0], "env") == 0)
		return (1);
	return (0);
}

int	ms_builtin_run_argv(t_ctx *ctx, char **argv)
{
	if (!argv || !argv[0])
		return (0);
	if (strcmp(argv[0], "echo") == 0)
		return (ms_builtin_echo(argv));
	if (strcmp(argv[0], "pwd") == 0)
		return (ms_builtin_pwd());
	if (strcmp(argv[0], "env") == 0)
		return (ms_builtin_env(ctx));
	return (1);
}
