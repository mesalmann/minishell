#include "../../minishell.h"

int	ms_builtin_unset(t_ctx *ctx, char **argv)
{
	int	i;
	int	ret;

	ret = 0;
	i = 1;
	while (argv[i])
	{
		if (!ms_is_valid_identifier(argv[i]))
		{
			ft_putstr_fd("minishell: unset: `", STDERR_FILENO);
			ft_putstr_fd(argv[i], STDERR_FILENO);
			ft_putendl_fd("': not a valid identifier", STDERR_FILENO);
			ret = 1;
		}
		else
			ms_env_unset(ctx, argv[i]);
		i++;
	}
	return (ret);
}
