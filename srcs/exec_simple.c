#include "../minishell.h"

static char *join_path(char *dir, char *cmd) {
  char *temp;
  char *full_path;

  temp = ft_strjoin(dir, "/");
  if (!temp)
    return (NULL);
  full_path = ft_strjoin(temp, cmd);
  free(temp);
  return (full_path);
}

char *ms_find_path(char *cmd, char **envp) {
  int i;
  char **paths;
  char *final_path;
  int permission_denied = 0;

  // 1. Durum: Doğrudan yol verilmişse (Slash varsa)
  if (ft_strchr(cmd, '/')) {
    if (access(cmd, F_OK) != 0)
      return (NULL); // errno otomatik ENOENT olur
    if (access(cmd, X_OK) != 0)
      return (NULL); // errno otomatik EACCES olur
    return (ft_strdup(cmd));
  }

  i = 0;
  while (envp[i] && ft_strncmp(envp[i], "PATH=", 5) != 0)
    i++;
  if (!envp[i]) {
    errno = ENOENT;
    return (NULL);
  }

  paths = ft_split(envp[i] + 5, ':');
  if (!paths)
    return (NULL);

  i = -1;
  while (paths[++i]) {
    final_path = join_path(paths[i], cmd);
    if (access(final_path, F_OK) == 0) // Dosya var mı?
    {
      if (access(final_path, X_OK) == 0) // Çalıştırılabilir mi?
      {
        free_tab(paths);
        return (final_path);
      }
      permission_denied = 1; // Dosyayı bulduk ama yetki yok!
    }
    free(final_path);
  }
  free_tab(paths);
  // Döngü bitti ve bulunamadıysa bilinçli errno set ediyoruz:
  if (permission_denied)
    errno = EACCES;
  else
    errno = ENOENT;
  return (NULL);
}

void ms_exec_simple(t_ctx *ctx, char **argv, char **envp) {
  char *path;
  pid_t pid;
  int status;

  if (ms_is_builtin_argv(argv)) {
    ctx->last_status = ms_builtin_run_argv(ctx, argv);
    return;
  }
  path = ms_find_path(argv[0], envp);
  if (!path) {
    ft_putstr_fd("minishell: ", 2);
    ft_putstr_fd(argv[0], 2);
    if (errno == EACCES) {
      ft_putendl_fd(": Permission denied", 2);
      ctx->last_status = 126;
    } else {
      ft_putendl_fd(": command not found", 2);
      ctx->last_status = 127;
    }
    return;
  }

  pid = fork();
  if (pid == 0) {
    if (execve(path, argv, envp) == -1) {
      perror("minishell");
      free(path);
      _exit(126);
    }
  } else if (pid > 0) {
    while (waitpid(pid, &status, 0) == -1 && errno == EINTR)
      continue;
    if (WIFEXITED(status))
      ctx->last_status = WEXITSTATUS(status);
    else if (WIFSIGNALED(status))
      ctx->last_status = 128 + WTERMSIG(status);
    free(path);
  } else {
    perror("minishell: fork");
    free(path);
    ctx->last_status = 1;
  }
}