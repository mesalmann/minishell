#include "../../minishell.h"
#include <string.h>

int ms_is_builtin_argv(char **argv)
{
    if (!argv || !argv[0])
        return (0);
    if (strcmp(argv[0], "pwd") == 0)
        return (1);
    // şimdilik sadece pwd
    // sonra: echo/env/cd/export/unset/exit ekleriz
    return (0);
}

int ms_builtin_run_argv(t_ctx *ctx, char **argv)
{
    (void)ctx;
    if (!argv || !argv[0])
        return (0);

    if (strcmp(argv[0], "pwd") == 0)
        return (ms_builtin_pwd());

    return (1);
}