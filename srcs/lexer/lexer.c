#include "../../minishell.h"

static int ms_is_end(char c);
static void skip_spaces(const char *line, int *i);
static int is_operator(char c);
static t_token *handle_operator(const char *line, int *i);
static t_token *handle_word(const char *line, int *i);
static char *read_word(const char *s, int *i);

t_token *token_new(t_tokkind kind, t_opkind op, char *lex) {
  t_token *tok;

  tok = malloc(sizeof(t_token));
  if (!tok)
    return (NULL);
  tok->kind = kind;
  tok->op = op;
  tok->lex = lex;
  tok->no_expand = false;
  tok->next = NULL;
  return (tok);
}

void ms_token_free(t_token *toks) {
  t_token *tmp;

  while (toks) {
    tmp = toks->next;
    if (toks->lex)
      free(toks->lex);
    free(toks);
    toks = tmp;
  }
}

static int is_operator(char c) { return (c == '<' || c == '>' || c == '|'); }

static void skip_spaces(const char *line, int *i) {
  while (line[*i] && isspace((unsigned char)line[*i]))
    (*i)++;
}

static int ms_is_end(char c) {
  return (isspace((unsigned char)c) || is_operator(c));
}

static t_token *handle_operator(const char *line, int *i) {
  t_opkind op;
  char c;

  op = OP_NONE;
  c = line[*i];
  if (c == '<') {
    if (line[*i + 1] == '<') {
      op = OP_HEREDOC;
      *i += 2;
    } else {
      op = OP_IN;
      *i += 1;
    }
  } else if (c == '>') {
    if (line[*i + 1] == '>') {
      op = OP_OUT_APPEND;
      *i += 2;
    } else {
      op = OP_OUT_TRUNC;
      *i += 1;
    }
  } else if (c == '|') {
    op = OP_PIPE;
    *i += 1;
  } else
    return (NULL);
  return (token_new(TK_OP, op, NULL));
}

/*
** Check if a word token is a quoted heredoc delimiter.
** Scans the entire string for any single or double quote => no_expand.
** Handles: 'EOF', "EOF", E"O"F, E'O'F, etc.
*/
static bool is_quoted_word(const char *s) {
  int i;

  if (!s)
    return (false);
  i = 0;
  while (s[i]) {
    if (s[i] == '\'' || s[i] == '"')
      return (true);
    i++;
  }
  return (false);
}

/*
** Strip ALL quotes from a heredoc delimiter string.
** 'EOF' -> EOF, E"O"F -> EOF, E'O'F -> EOF.
** Copies every character that is not a quote character.
*/
static char *strip_delim_quotes(const char *s) {
  char *out;
  int i;
  int k;

  if (!s)
    return (NULL);
  out = malloc(ft_strlen(s) + 1);
  if (!out)
    return (NULL);
  i = 0;
  k = 0;
  while (s[i]) {
    if (s[i] != '\'' && s[i] != '"')
      out[k++] = s[i];
    i++;
  }
  out[k] = '\0';
  return (out);
}

static t_token *handle_word(const char *line, int *i) {
  t_token *new;
  char *lex;

  lex = read_word(line, i);
  if (!lex)
    return (NULL);
  new = token_new(TK_WORD, OP_NONE, lex);
  if (!new) {
    free(lex);
    return (NULL);
  }
  return (new);
}

static int out_len(const char *s, int i) {
  t_qstate st;
  int n;

  st = Q_NONE;
  n = 0;
  while (s[i]) {
    if (st == Q_NONE) {
      if (ms_is_end(s[i]))
        break;
      if (s[i] == '\'') {
        st = Q_SINGLE;
        n++;
        i++;
        continue;
      }
      if (s[i] == '"') {
        st = Q_DOUBLE;
        n++;
        i++;
        continue;
      }
      n++;
      i++;
    } else if (st == Q_SINGLE) {
      if (s[i] == '\'') {
        st = Q_NONE;
        n++;
        i++;
        continue;
      }
      n++;
      i++;
    } else {
      if (s[i] == '"') {
        st = Q_NONE;
        n++;
        i++;
        continue;
      }
      n++;
      i++;
    }
  }
  if (st != Q_NONE)
    return (-1);
  return (n);
}

static char *read_word(const char *s, int *i) {
  t_qstate st;
  char *out;
  int n;
  int k;

  n = out_len(s, *i);
  if (n < 0)
    return (NULL);
  out = malloc((size_t)n + 1);
  if (!out)
    return (NULL);
  st = Q_NONE;
  k = 0;
  while (s[*i]) {
    if (st == Q_NONE) {
      if (ms_is_end(s[*i]))
        break;
      if (s[*i] == '\'') {
        st = Q_SINGLE;
        out[k++] = s[(*i)++];
        continue;
      }
      if (s[*i] == '"') {
        st = Q_DOUBLE;
        out[k++] = s[(*i)++];
        continue;
      }
      out[k++] = s[(*i)++];
    } else if (st == Q_SINGLE) {
      if (s[*i] == '\'') {
        st = Q_NONE;
        out[k++] = s[(*i)++];
        continue;
      }
      out[k++] = s[(*i)++];
    } else {
      if (s[*i] == '"') {
        st = Q_NONE;
        out[k++] = s[(*i)++];
        continue;
      }
      out[k++] = s[(*i)++];
    }
  }
  out[k] = '\0';
  return (out);
}

/*
** After building the token, if the previous token was OP_HEREDOC,
** mark this word token as no_expand and strip its outer quotes.
*/
static void mark_heredoc_delim(t_token *prev_op, t_token *word_tok) {
  char *stripped;

  if (!prev_op || prev_op->op != OP_HEREDOC)
    return;
  word_tok->no_expand = is_quoted_word(word_tok->lex);
  stripped = strip_delim_quotes(word_tok->lex);
  if (stripped) {
    free(word_tok->lex);
    word_tok->lex = stripped;
  }
}

static int	has_unmatched_quote(const char *s, int i)
{
	t_qstate	st;

	st = Q_NONE;
	while (s[i])
	{
		if (st == Q_NONE)
		{
			if (ms_is_end(s[i]))
				break ;
			if (s[i] == '\'')
				st = Q_SINGLE;
			else if (s[i] == '"')
				st = Q_DOUBLE;
		}
		else if (st == Q_SINGLE)
		{
			if (s[i] == '\'')
				st = Q_NONE;
		}
		else
		{
			if (s[i] == '"')
				st = Q_NONE;
		}
		i++;
	}
	if (st == Q_SINGLE)
		return ('\'');
	if (st == Q_DOUBLE)
		return ('"');
	return (0);
}

t_token *ms_tokenize(const char *line, t_ctx *ctx) {
  t_token *head;
  t_token *tail;
  t_token *new;
  int i;
  int uq;

  head = NULL;
  tail = NULL;
  i = 0;
  while (line && line[i]) {
    skip_spaces(line, &i);
    if (!line[i])
      break;
    if (is_operator(line[i]))
      new = handle_operator(line, &i);
    else {
      uq = has_unmatched_quote(line, i);
      new = handle_word(line, &i);
      if (!new && uq) {
        ms_token_free(head);
        ft_putstr_fd("minishell: unexpected EOF while looking for matching `",
                      2);
        write(2, &(char){(char)uq}, 1);
        ft_putendl_fd("'", 2);
        if (ctx)
          ctx->last_status = 2;
        return (NULL);
      }
    }
    if (!new) {
      ms_token_free(head);
      ft_putendl_fd("minishell: syntax error near unexpected token", 2);
      if (ctx)
        ctx->last_status = 2;
      return (NULL);
    }
    if (new->kind == TK_WORD && tail && tail->op == OP_HEREDOC)
      mark_heredoc_delim(tail, new);
    if (!head)
      head = new;
    else
      tail->next = new;
    tail = new;
  }
  return (head);
}