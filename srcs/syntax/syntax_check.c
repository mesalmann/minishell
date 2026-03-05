#include "../../minishell.h"

static bool check_first_token(t_token *tokens) {
  if (tokens && tokens->kind == TK_OP && tokens->op == OP_PIPE)
    return (false);
  return (true);
}

static bool check_last_token_and_adjacent(t_token *tokens) {
  t_token *curr = tokens;

  while (curr) {
    if (curr->kind == TK_OP) {

      // Redirection must be followed by WORD
      if (curr->op != OP_PIPE) {
        if (!curr->next || curr->next->kind != TK_WORD)
          return (false);
      }

      // Pipe must be followed by a command
      else {
        if (!curr->next || curr->next->kind != TK_WORD)
          return (false);
      }
    }
    curr = curr->next;
  }
  return (true);
}

bool ms_syntax_validate(t_token *tokens, t_ctx *ctx) {
  if (!tokens)
    return (true);

  if (!check_first_token(tokens) || !check_last_token_and_adjacent(tokens)) {
    ft_putendl_fd("minishell: syntax error near unexpected token", 2);
    if (ctx)
      ctx->last_status = 2;
    return (false);
  }
  return (true);
}

int ms_err_syntax(t_ctx *ctx, const char *msg) {
  ft_putstr_fd("minishell: ", 2);
  ft_putendl_fd(msg, 2);
  if (ctx)
    ctx->last_status = 2;
  return (2);
}
