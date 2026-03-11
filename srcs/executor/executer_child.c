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

static void	child_exit(t_ctx *ctx, char *path, int code)
{
    if (ctx)
    {
        if (ctx->cur_tokens)
        {
            ms_token_free(ctx->cur_tokens);
            ctx->cur_tokens = NULL;
        }
        if (ctx->cur_ast)
        {
            ms_cmd_free_list(ctx->cur_ast);
            ctx->cur_ast = NULL;
        }
    }
    ms_ctx_destroy(ctx);
    free(path);
    _exit(code);
}

static void	exec_sh_fallback(char **argv, char **envp)
{
	char	**nargv;
	int		argc;
	int		i;

	argc = 0;
	while (argv[argc])
		argc++;
	nargv = malloc(sizeof(char *) * (argc + 2));
	if (!nargv)
		return ;
	nargv[0] = "/bin/sh";
	i = 0;
	while (i <= argc)
	{
		nargv[i + 1] = argv[i];
		i++;
	}
	execve("/bin/sh", nargv, envp);
	free(nargv);
}

static void child_run_cmd(t_ctx *ctx, t_cmdnode *cmd)
{
    char		*path;
    int			ret;
    int			status;
    t_cmdnode	*node;
    t_cmdnode	*next;

    if (!cmd->argv || !cmd->argv[0])
        child_exit(ctx, NULL, 0);
    if (!ms_apply_redirs(cmd, NULL, NULL))
        child_exit(ctx, NULL, 1);
    if (ms_is_builtin_argv(cmd->argv))
    {
        ret = ms_builtin_run_argv(ctx, cmd->argv);
        status = (ret == -1) ? ctx->last_status : ret;
        child_exit(ctx, NULL, status);
    }
    if (ctx->env_dirty && !ms_env_build_envp(ctx))
        child_exit(ctx, NULL, 1);
    path = ms_resolve_path(ctx, cmd->argv[0]);
    if (!path)
    {
        ft_putstr_fd("minishell: ", STDERR_FILENO);
        ft_putstr_fd(cmd->argv[0], STDERR_FILENO);
        if (errno == EISDIR)
            ft_putendl_fd(": Is a directory", STDERR_FILENO);
        else if (errno == EACCES)
            ft_putendl_fd(": Permission denied", STDERR_FILENO);
        else if (ft_strchr(cmd->argv[0], '/'))
            ft_putendl_fd(": No such file or directory", STDERR_FILENO);
        else
            ft_putendl_fd(": command not found", STDERR_FILENO);
        child_exit(ctx, NULL, (errno == ENOENT) ? 127 : 126);
    }
    ms_token_free(ctx->cur_tokens);
    ctx->cur_tokens = NULL;
    node = ctx->cur_ast;
    while (node)
    {
        next = node->next;
        if (node != cmd)
            ms_cmd_free(node);
        node = next;
    }
    cmd->next = NULL;
    ctx->cur_ast = NULL;
    execve(path, cmd->argv, ctx->envp_cache);
    if (errno == ENOEXEC)
        exec_sh_fallback(cmd->argv, ctx->envp_cache);
    perror("minishell");
    ms_cmd_free(cmd);
    child_exit(ctx, path, 126);
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

static void	parent_close_used_ends(int *pipes, int n, int i)
{
	if (i < n - 1)
		close(pipes[i * 2 + 1]);
	if (i > 0)
		close(pipes[(i - 1) * 2]);
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
            free(pipes);
            free(pids);
            child_run_cmd(ctx, cmd);
        }
        parent_close_used_ends(pipes, n, i);
        cmd = cmd->next;
        i++;
    }
    return (1);
}
