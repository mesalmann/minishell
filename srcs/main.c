#include "../minishell.h"
volatile sig_atomic_t g_sig = 0;

void handle_sigint(int sig) {
  (void)sig;
  write(1, "\n", 1);

  rl_on_new_line();
  rl_replace_line("", 0);
  rl_redisplay();

  g_sig = SIGINT;
}

void ms_loop(t_ctx *ctx, char **envp) {
  char *line;
  t_token *tokens;
  t_cmdnode *ast;

  (void)envp;
  while (1) {
    if (g_sig == SIGINT) {
      ctx->last_status = 130;
      g_sig = 0;
    }
    line = readline("minishell$ ");
    if (line == NULL) {
      if (ctx->interactive)
        write(STDERR_FILENO, "exit\n", 5);
      break;
    }
    if (line[0] == '\0') {
      free(line);
      continue;
    }
    add_history(line);
    tokens = ms_tokenize(line, ctx);
    if (!tokens) {
      free(line);
      continue;
    }
    if (!ms_syntax_validate(tokens, ctx)) {
      ms_token_free(tokens);
      free(line);
      continue;
    }
    if (!ms_expand_tokens(tokens, ctx)) {
      ctx->last_status = 1;
      ms_token_free(tokens);
      free(line);
      continue;
    }
    ast = ms_parse(tokens, ctx);
    if (ast) {
      t_cmdnode *nxt;
      ms_execute_pipeline(ctx, ast);
      while (ast) {
        nxt = ast->next;
        ms_cmd_free(ast);
        ast = nxt;
      }
    }
    ms_token_free(tokens);
    free(line);
  }
}

int main(int ac, char **av, char **envp) {
  t_ctx ctx;

  (void)ac;
  (void)av;

  signal(SIGINT, handle_sigint); // Ctrl-C ile çıkış yapmayı sağlayan signal.
  signal(SIGQUIT, SIG_IGN);

  if (ms_ctx_init(&ctx, envp) == false)
    return 1;

  ms_loop(&ctx, envp);

  ms_ctx_destroy(&ctx);
  return (ctx.last_status);
}