#include "../../minishell.h"

/*
** ms_expand_tokens: traverses token list, expands TK_WORD tokens in-place.
** Rules:
**   - tok->no_expand == true  → heredoc delimiter, skip (no $VAR, no quote
*removal)
**   - tok->kind == TK_OP      → skip
**   - tok->kind == TK_WORD    → ms_expand_str → replace tok->lex
**
** Returns false on malloc failure (caller must free token list and bail out).
*/
bool ms_expand_tokens(t_token *tokens, t_ctx *ctx) {
  t_token *curr;
  char *expanded;

  curr = tokens;
  while (curr) {
    if (curr->kind != TK_WORD || curr->lex == NULL || curr->no_expand) {
      curr = curr->next;
      continue;
    }
    expanded = ms_expand_str(ctx, curr->lex, false);
    if (!expanded)
      return (false);
    free(curr->lex);
    curr->lex = expanded;
    curr = curr->next;
  }
  return (true);
}
