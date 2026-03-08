#include "../minishell.h"

static int	count_cmds(t_cmdnode *cmd)
{
	int	n;

	n = 0;
	while (cmd)
	{
		n++;
		cmd = cmd->next;
	}
	return (n);
}

/*
** Tüm pipe fd'lerini kapat (parent veya child'da çağrılır).
** pipes dizisi: [r0, w0, r1, w1, ...] formatında (count adet çift).
*/
static void	close_all_pipes(int *pipes, int count)
{
	int	i;

	i = 0;
	while (i < count)
	{
		close(pipes[i * 2]);
		close(pipes[i * 2 + 1]);
		i++;
	}
}

/*
** exec_child - Pipeline'daki i. komutun child sürecinde çalıştırılması.
** Geri dönmez: _exit ile sonlanır.
*/
static void	exec_child(t_ctx *ctx, t_cmdnode *cmd, int *pipes, int n, int i)
{
	char	*path;

	signal(SIGINT, SIG_DFL);
	signal(SIGQUIT, SIG_DFL);
	if (i > 0)
		dup2(pipes[(i - 1) * 2], STDIN_FILENO);
	if (i < n - 1)
		dup2(pipes[i * 2 + 1], STDOUT_FILENO);
	close_all_pipes(pipes, n - 1);
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

/*
** Pipe açma hatası alındığında, önceden açılanları kapat.
*/
static void	cleanup_partial_pipes(int *pipes, int opened_count)
{
	int	i;

	i = 0;
	while (i < opened_count)
	{
		close(pipes[i * 2]);
		close(pipes[i * 2 + 1]);
		i++;
	}
}

/*
** ms_exec_pipeline_multi - N komutlu pipeline'ı çalıştırır.
**
** 1. N-1 pipe aç.
** 2. Her komut için fork; child pipe fd'lerini bağlar ve komutu çalıştırır.
** 3. Parent tüm pipe fd'lerini kapatır.
** 4. Tüm child'ları bekle; $? için son komutun exit status'u kullanılır.
*/
static void	ms_exec_pipeline_multi(t_ctx *ctx, t_cmdnode *pipeline)
{
	int			n;
	int			*pipes;
	pid_t		*pids;
	t_cmdnode	*cmd;
	int			i;
	int			status;

	n = count_cmds(pipeline);
	pipes = malloc(sizeof(int) * 2 * (n - 1));
	pids = malloc(sizeof(pid_t) * n);
	if (!pipes || !pids)
	{
		free(pipes);
		free(pids);
		ctx->last_status = 1;
		return ;
	}
	i = 0;
	while (i < n - 1)
	{
		if (pipe(pipes + i * 2) < 0)
		{
			cleanup_partial_pipes(pipes, i);
			free(pipes);
			free(pids);
			ctx->last_status = 1;
			return ;
		}
		i++;
	}
	cmd = pipeline;
	i = 0;
	while (i < n)
	{
		pids[i] = fork();
		if (pids[i] < 0)
		{
			close_all_pipes(pipes, n - 1);
			while (--i >= 0)
				waitpid(pids[i], &status, 0);
			free(pipes);
			free(pids);
			ctx->last_status = 1;
			return ;
		}
		if (pids[i] == 0)
			exec_child(ctx, cmd, pipes, n, i);
		cmd = cmd->next;
		i++;
	}
	close_all_pipes(pipes, n - 1);
	i = 0;
	while (i < n)
	{
		while (waitpid(pids[i], &status, 0) == -1 && errno == EINTR)
			continue ;
		if (i == n - 1)
		{
			if (WIFEXITED(status))
				ctx->last_status = WEXITSTATUS(status);
			else if (WIFSIGNALED(status))
				ctx->last_status = 128 + WTERMSIG(status);
		}
		i++;
	}
	free(pipes);
	free(pids);
}

void	ms_execute_pipeline(t_ctx *ctx, t_cmdnode *pipeline)
{
	if (!pipeline)
		return ;
	if (!ms_run_heredocs(ctx, pipeline))
	{
		ctx->last_status = 130;
		return ;
	}
	if (!pipeline->next)
	{
		ms_exec_simple(ctx, pipeline);
		return ;
	}
	ms_exec_pipeline_multi(ctx, pipeline);
}
