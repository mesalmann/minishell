#include "../minishell.h"

/*
** parser.c — single responsibility: produce t_cmdnode AST from a validated,
** expanded token list. No syntax checking here (done by ms_syntax_validate).
** No fd/pipe/fork info stored. No file opens.
*/

static bool is_redir_op(t_opkind op);
static t_cmdnode *cmd_new(void);
static t_redir *redir_new(t_redirtype type, char *target);
static t_heredoc *heredoc_new(char *delim, bool expand_mode);
static void redir_add_back(t_redir **lst, t_redir *new);
static void heredoc_add_back(t_heredoc **lst, t_heredoc *new);
static void free_argv_partial(char **argv, int filled);
static void free_cmd_chain(t_cmdnode *head);
static int count_argv_words(t_token *t);
static bool fill_argv(t_cmdnode *cmd, t_token *t);
static bool parse_ios(t_cmdnode *cmd, t_token *t);

static bool is_redir_op(t_opkind op) {
  return (op == OP_IN || op == OP_OUT_TRUNC || op == OP_OUT_APPEND);
}

static t_cmdnode *cmd_new(void) {
  t_cmdnode *c;

  c = malloc(sizeof(t_cmdnode));
  if (!c)
    return (NULL);
  c->argv = NULL;
  c->redirs = NULL;
  c->heredocs = NULL;
  c->next = NULL;
  return (c);
}

static t_redir *redir_new(t_redirtype type, char *target) {
  t_redir *r;

  r = malloc(sizeof(t_redir));
  if (!r)
    return (NULL);
  r->type = type;
  r->target = target;
  r->next = NULL;
  return (r);
}

static t_heredoc *heredoc_new(char *delim, bool expand_mode) {
  t_heredoc *h;

  h = malloc(sizeof(t_heredoc));
  if (!h)
    return (NULL);
  h->delim = delim;
  h->expand_mode = expand_mode;
  h->pipe_rd = -1;
  h->pipe_wr = -1;
  h->next = NULL;
  return (h);
}

static void redir_add_back(t_redir **lst, t_redir *new) {
  t_redir *t;

  if (!lst || !new)
    return;
  if (!*lst) {
    *lst = new;
    return;
  }
  t = *lst;
  while (t->next)
    t = t->next;
  t->next = new;
}

static void heredoc_add_back(t_heredoc **lst, t_heredoc *new) {
  t_heredoc *t;

  if (!lst || !new)
    return;
  if (!*lst) {
    *lst = new;
    return;
  }
  t = *lst;
  while (t->next)
    t = t->next;
  t->next = new;
}

static void free_argv_partial(char **argv, int filled) {
  int i;

  if (!argv)
    return;
  i = 0;
  while (i < filled) {
    free(argv[i]);
    i++;
  }
  free(argv);
}

static void free_cmd_chain(t_cmdnode *head) {
  t_cmdnode *tmp;

  while (head) {
    tmp = head->next;
    ms_cmd_free(head);
    head = tmp;
  }
}

/*
** Count WORD tokens in this command segment (up to the next PIPE).
** Skip the WORD that immediately follows a redir/heredoc operator
** (it belongs to the redir, not to argv).
*/
static int count_argv_words(t_token *t) {
  int count;

  count = 0;
  while (t && !(t->kind == TK_OP && t->op == OP_PIPE)) {
    if (t->kind == TK_WORD) {
      count++;
      t = t->next;
      continue;
    }
    if (t->kind == TK_OP && (is_redir_op(t->op) || t->op == OP_HEREDOC)) {
      t = t->next;
      if (t)
        t = t->next;
      continue;
    }
    t = t->next;
  }
  return (count);
}

static bool fill_argv(t_cmdnode *cmd, t_token *t) {
  int i;
  int n;

  n = count_argv_words(t);
  cmd->argv = malloc(sizeof(char *) * (n + 1));
  if (!cmd->argv)
    return (false);
  i = 0;
  while (t && !(t->kind == TK_OP && t->op == OP_PIPE)) {
    if (t->kind == TK_WORD) {
      cmd->argv[i] = ft_strdup(t->lex);
      if (!cmd->argv[i]) {
        free_argv_partial(cmd->argv, i);
        cmd->argv = NULL;
        return (false);
      }
      i++;
      t = t->next;
      continue;
    }
    if (t->kind == TK_OP && (is_redir_op(t->op) || t->op == OP_HEREDOC)) {
      t = t->next;
      if (t)
        t = t->next;
      continue;
    }
    t = t->next;
  }
  cmd->argv[i] = NULL;
  return (true);
}

/*
** parse_ios: extract redirs and heredocs from the command's token segment.
** expand_mode is taken directly from tok->no_expand (set by lexer).
** Delimiter string has already been quote-stripped by the lexer.
** No file is opened here. No fd is set. Only data structures are built.
*/
static bool parse_ios(t_cmdnode *cmd, t_token *t) {
  t_redirtype rt;
  t_redir *r;
  t_heredoc *h;
  char *delim;

  while (t && !(t->kind == TK_OP && t->op == OP_PIPE)) {
    if (t->kind == TK_OP && is_redir_op(t->op)) {
      rt = RD_IN;
      if (t->op == OP_OUT_TRUNC)
        rt = RD_OUT_TRUNC;
      else if (t->op == OP_OUT_APPEND)
        rt = RD_OUT_APPEND;
      t = t->next;
      r = redir_new(rt, ft_strdup(t->lex));
      if (!r || !r->target) {
        if (r)
          free(r);
        return (false);
      }
      redir_add_back(&cmd->redirs, r);
      t = t->next;
      continue;
    }
    if (t->kind == TK_OP && t->op == OP_HEREDOC) {
      t = t->next;
      /* no_expand=true means delimiter was quoted → no $ expansion */
      delim = ft_strdup(t->lex);
      if (!delim)
        return (false);
      h = heredoc_new(delim, !t->no_expand);
      if (!h) {
        free(delim);
        return (false);
      }
      heredoc_add_back(&cmd->heredocs, h);
      t = t->next;
      continue;
    }
    t = t->next;
  }
  return (true);
}

/*
** ms_parse: assumes tokens are already syntax-validated and expanded.
** Each call to ms_cmd_free / free_cmd_chain is NULL-safe.
*/
t_cmdnode *ms_parse(t_token *toks, t_ctx *ctx) {
  t_cmdnode *head;
  t_cmdnode *tail;
  t_cmdnode *cmd;

  (void)ctx;
  if (!toks)
    return (NULL);
  head = NULL;
  tail = NULL;
  while (toks) {
    cmd = cmd_new();
    if (!cmd) {
      free_cmd_chain(head);
      return (NULL);
    }
    if (!fill_argv(cmd, toks) || !parse_ios(cmd, toks)) {
      ms_cmd_free(cmd);
      free_cmd_chain(head);
      return (NULL);
    }
    if (!head)
      head = cmd;
    else
      tail->next = cmd;
    tail = cmd;
    while (toks && !(toks->kind == TK_OP && toks->op == OP_PIPE))
      toks = toks->next;
    if (toks && toks->op == OP_PIPE)
      toks = toks->next;
  }
  return (head);
}

/*
** ms_cmd_free: frees a SINGLE t_cmdnode.
** Caller must iterate the chain manually if freeing a full pipeline.
** NULL-safe on cmd itself.
*/
void ms_cmd_free(t_cmdnode *cmd) {
  t_redir *r;
  t_redir *rtmp;
  t_heredoc *h;
  t_heredoc *htmp;
  int i;

  if (!cmd)
    return;
  if (cmd->argv) {
    i = 0;
    while (cmd->argv[i])
      free(cmd->argv[i++]);
    free(cmd->argv);
  }
  r = cmd->redirs;
  while (r) {
    rtmp = r->next;
    free(r->target);
    free(r);
    r = rtmp;
  }
  h = cmd->heredocs;
  while (h) {
    htmp = h->next;
    if (h->pipe_rd >= 0)
      close(h->pipe_rd);
    if (h->pipe_wr >= 0)
      close(h->pipe_wr);
    free(h->delim);
    free(h);
    h = htmp;
  }
  free(cmd);
}