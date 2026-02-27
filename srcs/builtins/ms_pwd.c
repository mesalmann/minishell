#include "minishell.h"

int	ms_builtin_pwd(void)
{
	char	cwd[1024];

	if (getcwd(cwd, sizeof(cwd)) != NULL)
	{
		printf("%s\n", cwd);
		return (0); 
	}
	perror("pwd");
	return (1); 
}