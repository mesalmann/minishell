/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   syntax_check.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mesalman <mesalman@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/18 17:01:28 by mesalman          #+#    #+#             */
/*   Updated: 2026/03/08 00:00:00 by mesalman         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../minishell.h"

/*
** tok_name: token için bash tarzı hata mesajı üret.
** OP_PIPE   → "|"
** OP_IN     → "<"  vb.
** NULL (son token sonrası) → "newline"
*/
static const char *tok_name(t_token *tok) {
  if (!tok)
    return ("newline");
  if (tok->kind == TK_OP) {
    if (tok->op == OP_PIPE)
      return ("|");
    if (tok->op == OP_IN)
      return ("<");
    if (tok->op == OP_OUT_TRUNC)
      return (">");
    if (tok->op == OP_OUT_APPEND)
      return (">>");
    if (tok->op == OP_HEREDOC)
      return ("<<");
  }
  return ("word");
}

/*
** syntax_err: bash formatında syntax error mesajı basar ve false döner.
** "minishell: syntax error near unexpected token `X'"
*/
static bool syntax_err(t_ctx *ctx, t_token *near_tok) {
  ft_putstr_fd("minishell: syntax error near unexpected token `", 2);
  ft_putstr_fd(tok_name(near_tok), 2);
  ft_putendl_fd("'", 2);
  if (ctx)
    ctx->last_status = 2;
  return (false);
}

/*
** ms_syntax_validate: tek geçişte tüm syntax kurallarını kontrol eder.
**
** Kurallar:
**  1. İlk token OP_PIPE → hata (| cmd)
**  2. Her OP (pipe veya redir) sonrasında:
**       - NULL → hata (son token operatör)
**       - Diğer OP → hata (operator operator)
**         Özel: redir bölümünde redir sonrası başka redir gelemez,
**               ama pipe sonrası redir gelebilir (echo | > out → bu bash'ta
**               da syntax error, aynı kural geçerli).
**  3. OP_PIPE sonrası OP_PIPE → hata (|| cmd — bashta farklı op, bizde error)
**  4. Redir OP sonrası TK_WORD dışı → hata
*/
bool ms_syntax_validate(t_token *tokens, t_ctx *ctx) {
  t_token *curr;

  if (!tokens)
    return (true);
  /* Kural 1: ilk token pipe olamaz */
  if (tokens->kind == TK_OP && tokens->op == OP_PIPE)
    return (syntax_err(ctx, tokens));
  curr = tokens;
  while (curr) {
    if (curr->kind != TK_OP) {
      curr = curr->next;
      continue;
    }
    /* curr bir operatör → sonrakine bak */
    if (!curr->next) {
      /* Kural 2a: son token operatör → "newline" */
      return (syntax_err(ctx, NULL));
    }
    if (curr->next->kind == TK_OP) {
      /* Kural 2b / 3: operatör sonrası operatör */
      return (syntax_err(ctx, curr->next));
    }
    curr = curr->next;
  }
  return (true);
}

/* ── Hata yardımcıları ──────────────────────────────────────────────────── */

int ms_err_syntax(t_ctx *ctx, const char *msg) {
  ft_putstr_fd("minishell: ", 2);
  ft_putendl_fd(msg, 2);
  if (ctx)
    ctx->last_status = 2;
  return (2);
}

int ms_err_errno(t_ctx *ctx, const char *where) {
  ft_putstr_fd("minishell: ", 2);
  ft_putstr_fd(where, 2);
  ft_putstr_fd(": ", 2);
  ft_putendl_fd(strerror(errno), 2);
  if (ctx)
    ctx->last_status = 1;
  return (1);
}
