/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_cmd.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hdere <hdere@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/08 12:00:00 by hdere             #+#    #+#             */
/*   Updated: 2026/03/08 07:35:02 by hdere            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static void exec_builtin_simple(t_ctx *ctx, t_cmdnode *cmd)
{
    int saved_in;
    int saved_out;
    int ret;

    saved_in = -1;
    saved_out = -1;
    if (!ms_apply_redirs(cmd, &saved_in, &saved_out))
    {
        ms_restore_stdio(saved_in, saved_out);
        ctx->last_status = 1;
        return;
    }

    if (cmd->argv && strcmp(cmd->argv[0], "exit") == 0 && ctx->interactive)
        ft_putendl_fd("exit", STDERR_FILENO);

    ret = ms_builtin_run_argv(ctx, cmd->argv);

    if (ret == -1)
    {
        ms_restore_stdio(saved_in, saved_out);  
        ms_ctx_destroy(ctx);
        rl_clear_history();
        exit(ctx->last_status);
    }
    
    ctx->last_status = ret;
    ms_restore_stdio(saved_in, saved_out);
}

static void exec_cmd_not_found(t_ctx *ctx, char *name)
{
    ft_putstr_fd("minishell: ", 2);
    ft_putstr_fd(name, 2);
    if (errno == EACCES)
    {
        ft_putendl_fd(": Permission denied", 2);
        ctx->last_status = 126;
    }
    else
    {
        ft_putendl_fd(": command not found", 2);
        ctx->last_status = 127;
    }
}

static void exec_child_process(t_cmdnode *cmd, char *path, char **envp)
{
    signal(SIGINT, SIG_DFL);
    signal(SIGQUIT, SIG_DFL);
    if (!ms_apply_redirs(cmd, NULL, NULL))
    {
        free(path);
        _exit(1);
    }
    execve(path, cmd->argv, envp);
    perror("minishell");
    free(path);
    _exit(126);
}

static void exec_parent_wait(t_ctx *ctx, pid_t pid, char *path)
{
    int status;

    if (pid < 0)
    {
        perror("minishell: fork");
        free(path);
        ctx->last_status = 1;
        return;
    }
    while (waitpid(pid, &status, 0) == -1 && errno == EINTR)
        continue;
    if (WIFEXITED(status))
        ctx->last_status = WEXITSTATUS(status);
    else if (WIFSIGNALED(status))
        ctx->last_status = 128 + WTERMSIG(status);
    free(path);
}

void ms_exec_simple(t_ctx *ctx, t_cmdnode *cmd)
{
    char *path;
    pid_t pid;
    if (!cmd->argv || !cmd->argv[0])
    {
        int s_in = -1;
        int s_out = -1;
        if (ms_apply_redirs(cmd, &s_in, &s_out))
            ctx->last_status = 0;
        else
            ctx->last_status = 1;
        ms_restore_stdio(s_in, s_out);
        return;
    }

    if (ms_is_builtin_argv(cmd->argv))
    {
        exec_builtin_simple(ctx, cmd);
        return;
    }
    if (ctx->env_dirty && !ms_env_build_envp(ctx))
    {
        ft_putendl_fd("minishell: fatal: env rebuild failed", 2);
        ctx->last_status = 1;
        return;
    }
    path = ms_resolve_path(ctx, cmd->argv[0]);
    if (!path)
    {
        exec_cmd_not_found(ctx, cmd->argv[0]);
        return;
    }
    pid = fork();
    if (pid == 0)
        exec_child_process(cmd, path, ctx->envp_cache);
    else
        exec_parent_wait(ctx, pid, path);
}
