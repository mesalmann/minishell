#include "minishell.h"

t_bltin	ms_builtin_kind(const char *name)
{
	if (!name) return (BI_NONE);
	if (ft_strncmp(name, "echo", 5) == 0) return (BI_ECHO);
	if (ft_strncmp(name, "cd", 3) == 0) return (BI_CD);
	if (ft_strncmp(name, "pwd", 4) == 0) return (BI_PWD);
	if (ft_strncmp(name, "export", 7) == 0) return (BI_EXPORT);
	if (ft_strncmp(name, "unset", 6) == 0) return (BI_UNSET);
	if (ft_strncmp(name, "env", 4) == 0) return (BI_ENV);
	if (ft_strncmp(name, "exit", 5) == 0) return (BI_EXIT);
	return (BI_NONE);
}

int	ms_builtin_run(t_ctx *ctx, char **argv)
{
	t_bltin kind = ms_builtin_kind(argv[0]);

	if (kind == BI_ECHO)
		return (ms_builtin_echo(argv));
	if (kind == BI_PWD)
		return (ms_builtin_pwd());
	if (kind == BI_CD)
		return (ms_builtin_cd(ctx, argv));
	// Diğerlerini buraya ekleyeceksin...
	return (1);
}