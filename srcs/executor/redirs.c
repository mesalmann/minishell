#include "../minishell.h"

/* ── helpers: open ─────────────────────────────────────────────────────── */

static int	open_redir_fd(t_redir *r)
{
	if (r->type == RD_IN)
		return (open(r->target, O_RDONLY));
	if (r->type == RD_OUT_TRUNC)
		return (open(r->target, O_WRONLY | O_CREAT | O_TRUNC, 0644));
	return (open(r->target, O_WRONLY | O_CREAT | O_APPEND, 0644));
}

/* ── helpers: error cleanup ─────────────────────────────────────────────── */

/*
** restore_and_clear - hata yolunda:
**   1. Önceki iterasyonlarda dup2 ile değişen stdio'yu geri yükler.
**   2. saved fd'leri kapatır.
**   3. Pointer'ları -1'e sıfırlar → caller'ın ms_restore_stdio'su no-op olur.
*/
static void	restore_and_clear(int *saved_stdin, int *saved_stdout)
{
	if (saved_stdin && *saved_stdin >= 0)
	{
		dup2(*saved_stdin, STDIN_FILENO);
		close(*saved_stdin);
		*saved_stdin = -1;
	}
	if (saved_stdout && *saved_stdout >= 0)
	{
		dup2(*saved_stdout, STDOUT_FILENO);
		close(*saved_stdout);
		*saved_stdout = -1;
	}
}

/* ── ms_apply_redirs ────────────────────────────────────────────────────── */

/*
** ms_apply_redirs - cmd->redirs ve cmd->heredocs'u sırayla uygular.
**
** Sıralama kararı:
**   1. cmd->redirs listesi (< > >>) sırayla uygulanır → son kazanır.
**   2. cmd->heredocs varsa, listenin SON heredoc'unun pipe_rd'si
**      stdin'e dup2 edilir → << her zaman < sonrasında işlenir.
**   3. Tüm heredoc pipe_rd'leri kapatılır.
**
** Hata (open/dup2 fail):
**   - Stderr'e mesaj basılır.
**   - restore_and_clear ile stdio orijinal haline döner.
**   - false döner; caller ms_restore_stdio(-1,-1) çağırabilir (no-op).
**
** Child process için saved_* NULL geçilir (restore gerekmez, _exit yapılır).
*/
bool	ms_apply_redirs(t_cmdnode *cmd, int *saved_stdin, int *saved_stdout)
{
	t_redir		*r;
	t_heredoc	*h;
	t_heredoc	*last_hd;
	int			fd;
	int			target_fd;

	if (saved_stdin)
		*saved_stdin = dup(STDIN_FILENO);
	if (saved_stdout)
		*saved_stdout = dup(STDOUT_FILENO);
	r = cmd->redirs;
	while (r)
	{
		fd = open_redir_fd(r);
		if (fd == -1)
		{
			ft_putstr_fd("minishell: ", 2);
			ft_putstr_fd(r->target, 2);
			ft_putstr_fd(": ", 2);
			ft_putendl_fd(strerror(errno), 2);
			restore_and_clear(saved_stdin, saved_stdout);
			return (false);
		}
		target_fd = (r->type == RD_IN) ? STDIN_FILENO : STDOUT_FILENO;
		if (dup2(fd, target_fd) == -1)
		{
			perror("minishell: dup2");
			close(fd);
			restore_and_clear(saved_stdin, saved_stdout);
			return (false);
		}
		close(fd);
		r = r->next;
	}
	h = cmd->heredocs;
	last_hd = NULL;
	while (h)
	{
		last_hd = h;
		h = h->next;
	}
	if (last_hd && last_hd->pipe_rd >= 0)
	{
		if (dup2(last_hd->pipe_rd, STDIN_FILENO) == -1)
		{
			perror("minishell: dup2");
			h = cmd->heredocs;
			while (h)
			{
				if (h->pipe_rd >= 0)
				{
					close(h->pipe_rd);
					h->pipe_rd = -1;
				}
				h = h->next;
			}
			restore_and_clear(saved_stdin, saved_stdout);
			return (false);
		}
	}
	h = cmd->heredocs;
	while (h)
	{
		if (h->pipe_rd >= 0)
		{
			close(h->pipe_rd);
			h->pipe_rd = -1;
		}
		h = h->next;
	}
	return (true);
}

/* ── ms_restore_stdio ───────────────────────────────────────────────────── */

void	ms_restore_stdio(int saved_stdin, int saved_stdout)
{
	if (saved_stdin >= 0)
	{
		dup2(saved_stdin, STDIN_FILENO);
		close(saved_stdin);
	}
	if (saved_stdout >= 0)
	{
		dup2(saved_stdout, STDOUT_FILENO);
		close(saved_stdout);
	}
}

/* ── heredoc helpers ────────────────────────────────────────────────────── */

/*
** write_hd_line - bir satırı pipe'a yazar.
** expand_mode:true → $VAR/$? expand edilir; false → ham yazılır.
** Başarıda true, malloc/write hatasında false döner.
*/
static bool	write_hd_line(t_ctx *ctx, t_heredoc *h, char *line)
{
	char	*expanded;
	size_t	len;

	if (h->expand_mode)
	{
		expanded = ms_expand_str(ctx, line, false);
		if (!expanded)
			return (false);
		len = ft_strlen(expanded);
		write(h->pipe_wr, expanded, len);
		write(h->pipe_wr, "\n", 1);
		free(expanded);
	}
	else
	{
		len = ft_strlen(line);
		write(h->pipe_wr, line, len);
		write(h->pipe_wr, "\n", 1);
	}
	return (true);
}

/*
** read_one_heredoc - tek bir heredoc yapısı için pipe oluşturur ve doldurur.
**
** Akış:
**   1. pipe() ile çift yönlü fd çifti aç.
**   2. readline("> ") ile satır satır oku.  add_history() ÇAĞIRILMAZ.
**   3. readline() NULL dönerse (Ctrl-D / EOF): uyarı bas, döngüden çık.
**   4. Satır == delimiter → döngüden çık.
**   5. expand_mode'a göre satırı yaz.
**   6. pipe write end'i kapat → okuyucu EOF görür.
**
** Başarıda true (h->pipe_rd artık geçerli), hata pipe'ta false.
*/
static bool	read_one_heredoc(t_ctx *ctx, t_heredoc *h)
{
	int		pipefd[2];
	char	*line;

	if (pipe(pipefd) == -1)
	{
		perror("minishell: pipe");
		return (false);
	}
	h->pipe_rd = pipefd[0];
	h->pipe_wr = pipefd[1];
	while (1)
	{
		line = readline("> ");
		if (g_sig == SIGINT)
		{
			free(line);
			g_sig = 0;
			close(h->pipe_wr);
			h->pipe_wr = -1;
			return (false);
		}
		if (!line)
		{
			ft_putstr_fd("minishell: warning: here-document at EOF\n", 2);
			break ;
		}
		if (strcmp(line, h->delim) == 0)
		{
			free(line);
			break ;
		}
		if (!write_hd_line(ctx, h, line))
		{
			free(line);
			close(h->pipe_wr);
			h->pipe_wr = -1;
			return (false);
		}
		free(line);
	}
	close(h->pipe_wr);
	h->pipe_wr = -1;
	return (true);
}

/* ── ms_run_heredocs ────────────────────────────────────────────────────── */

/*
** ms_run_heredocs - pipeline'daki tüm komutların tüm heredoc'larını okur.
**
** Fork'tan ÖNCE parent'ta çağrılır; böylece:
**   - readline() etkileşimi çalışır (terminal parent'ta).
**   - Ctrl-C/Ctrl-D ile heredoc iptal edilirse komut çalıştırılmaz.
**
** Birden fazla heredoc varsa HEPSİ okunur (kullanıcı tüm delimiter'ları
** girmek zorundadır), ama stdin'e yalnızca son heredoc bağlanır.
**
** Başarıda true; herhangi bir pipe() hatası veya write_hd_line hatası
** durumunda false döner.
*/
bool	ms_run_heredocs(t_ctx *ctx, t_cmdnode *pipeline)
{
	t_cmdnode	*cmd;
	t_heredoc	*h;

	cmd = pipeline;
	while (cmd)
	{
		h = cmd->heredocs;
		while (h)
		{
			if (!read_one_heredoc(ctx, h))
				return (false);
			h = h->next;
		}
		cmd = cmd->next;
	}
	return (true);
}
