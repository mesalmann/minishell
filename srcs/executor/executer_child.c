/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   executer_child.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hdere <hdere@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/08 12:00:00 by hdere             #+#    #+#             */
/*   Updated: 2026/03/08 07:37:59 by hdere            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static void child_setup_io(int *pipes, int n, int i)
{
    signal(SIGINT, SIG_DFL);
    signal(SIGQUIT, SIG_DFL);
    if (i > 0)
        dup2(pipes[(i - 1) * 2], STDIN_FILENO);
    if (i < n - 1)
        dup2(pipes[i * 2 + 1], STDOUT_FILENO);
    ms_close_all_pipes(pipes, n - 1);
}

static void child_run_cmd(t_ctx *ctx, t_cmdnode *cmd)
{
    char *path;

    if (!ms_apply_redirs(cmd, NULL, NULL))
        _exit(1);
    if (ms_is_builtin_argv(cmd->argv))
        _exit(ms_builtin_run_argv(ctx, cmd->argv));
    if (ctx->env_dirty && !ms_env_build_envp(ctx))
        _exit(1);
    path = ms_resolve_path(ctx, cmd->argv[0]);
    if (!path)
    {
        ft_putstr_fd("minishell: ", STDERR_FILENO);
        ft_putstr_fd(cmd->argv[0], STDERR_FILENO);
        if (errno == EACCES)
        {
            ft_putendl_fd(": Permission denied", STDERR_FILENO);
            _exit(126);
        }
        ft_putendl_fd(": command not found", STDERR_FILENO);
        _exit(127);
    }
    execve(path, cmd->argv, ctx->envp_cache);
    perror("minishell");
    free(path);
    _exit(126);
}

static int open_pipes(int *pipes, int count)
{
    int i;

    i = 0;
    while (i < count)
    {
        if (pipe(pipes + i * 2) < 0)
        {
            ms_close_all_pipes(pipes, i);
            return (0);
        }
        i++;
    }
    return (1);
}

static int fork_handle_error(pid_t *pids, int done, int *pipes, int n)
{
    int status;
    int j;

    ms_close_all_pipes(pipes, n - 1);
    j = 0;
    while (j < done)
    {
        waitpid(pids[j], &status, 0);
        j++;
    }
    return (0);
}

int ms_create_pipeline(t_ctx *ctx, t_cmdnode *pl, int *pipes, pid_t *pids)
{
    t_cmdnode *cmd;
    int n;
    int i;

    n = ms_count_cmds(pl);
    if (!open_pipes(pipes, n - 1))
        return (0);
    cmd = pl;
    i = 0;
    while (i < n)
    {
        pids[i] = fork();
        if (pids[i] < 0)
            return (fork_handle_error(pids, i, pipes, n));
        if (pids[i] == 0)
        {
            child_setup_io(pipes, n, i);
            child_run_cmd(ctx, cmd);
        }
        cmd = cmd->next;
        i++;
    }
    return (1);
}
