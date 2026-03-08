#include "../../minishell.h"

static int	cd_error(const char *path)
{
	ft_putstr_fd("minishell: cd: ", STDERR_FILENO);
	ft_putstr_fd(path, STDERR_FILENO);
	ft_putstr_fd(": ", STDERR_FILENO);
	ft_putendl_fd(strerror(errno), STDERR_FILENO);
	return (1);
}

static char	*cd_get_home(t_ctx *ctx)
{
	char	*home;

	home = ms_env_get(ctx, "HOME");
	if (!home)
	{
		ft_putendl_fd("minishell: cd: HOME not set", STDERR_FILENO);
		return (NULL);
	}
	return (ft_strdup(home));
}

static char	*resolve_cd_path(t_ctx *ctx, char **argv, int *print_path)
{
	char	*path;

	*print_path = 0;
	if (!argv[1] || strcmp(argv[1], "~") == 0)
		return (cd_get_home(ctx));
	if (strcmp(argv[1], "-") == 0)
	{
		path = ms_env_get(ctx, "OLDPWD");
		if (!path)
		{
			ft_putendl_fd("minishell: cd: OLDPWD not set", STDERR_FILENO);
			return (NULL);
		}
		*print_path = 1;
		return (ft_strdup(path));
	}
	if (argv[1][0] == '~' && argv[1][1] == '/')
	{
		path = ms_env_get(ctx, "HOME");
		if (!path)
		{
			ft_putendl_fd("minishell: cd: HOME not set", STDERR_FILENO);
			return (NULL);
		}
		return (ft_strjoin(path, argv[1] + 1));
	}
	return (ft_strdup(argv[1]));
}

int	ms_builtin_cd(t_ctx *ctx, char **argv)
{
	char	*path;
	char	oldpwd[4096];
	char	newpwd[4096];
	int		print_path;

	if (argv[1] && argv[2])
	{
		ft_putendl_fd("minishell: cd: too many arguments", STDERR_FILENO);
		return (1);
	}
	if (!getcwd(oldpwd, sizeof(oldpwd)))
		oldpwd[0] = '\0';
	path = resolve_cd_path(ctx, argv, &print_path);
	if (!path)
		return (1);
	if (chdir(path) != 0)
	{
		cd_error(path);
		free(path);
		return (1);
	}
	if (print_path)
		ft_putendl_fd(path, STDOUT_FILENO);
	free(path);
	ms_env_set(ctx, "OLDPWD", oldpwd, true);
	if (getcwd(newpwd, sizeof(newpwd)))
		ms_env_set(ctx, "PWD", newpwd, true);
	return (0);
}
