#include "minishell.h"

static char *join_path(char *dir, char *cmd)
{
    char *temp;
    char *full_path;

    temp = ft_strjoin(dir, "/");
    if (!temp) return (NULL);
    full_path = ft_strjoin(temp, cmd);
    free(temp);
    return (full_path);
}

char *ms_find_path(char *cmd, char **envp)
{
    int     i;
    char    **paths;
    char    *final_path;
    if (ft_strchr(cmd, '/')) 
    {
        if (access(cmd, F_OK) != 0)
            return (NULL);
        if (access(cmd, X_OK) != 0)
        {
            ft_putstr_fd("minishell: permission denied ", 2);
            ft_putendl_fd(cmd, 2);
            return (NULL);
        }
        return (ft_strdup(cmd));
    }
    i = 0;
    while (envp[i] && ft_strncmp(envp[i], "PATH=", 5) != 0)
        i++;
    if (!envp[i]) return (NULL);

    paths = ft_split(envp[i] + 5, ':');
    if (!paths) return (NULL);

    i = -1;
while (paths[++i])
    {
        final_path = join_path(paths[i], cmd);
        if (access(final_path, X_OK) == 0)
        {
            free_tab(paths);
            return (final_path);
        }
        free(final_path);
    }
    free_tab(paths);
    return (NULL);
}

void ms_exec_simple(t_ctx *ctx, char **argv, char **envp)
{
    char    *path;
    pid_t   pid;
    int     status;
    
    if (ms_builtin_kind(argv[0]) != BI_NONE)
    {
        ctx->last_status = ms_builtin_run(ctx, argv);
        return ; 
    }

    path = ms_find_path(argv[0], envp);
  if (!path)
    {
        ft_putstr_fd("minishell: ", 2);
        ft_putstr_fd(argv[0], 2);
        ft_putendl_fd(": command not found", 2);
        ctx->last_status = 127;
        return ;
    }

    pid = fork();
    if (pid == 0)
    {

        if (execve(path, argv, envp) == -1)
        {
            perror("minishell"); 
            free(path); 
            _exit(126); 
        }
    }
else if (pid > 0)
    {
        waitpid(pid, &status, 0);
        
        if (WIFEXITED(status))
            ctx->last_status = WEXITSTATUS(status);
        else if (WIFSIGNALED(status))
            ctx->last_status = 128 + WTERMSIG(status);
        
        free(path);
    }
    else
        perror("fork");
}
