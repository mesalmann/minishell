#include "../../minishell.h"

static bool sb_grow(char **buf, size_t *cap, size_t need);
static bool sb_pushc(char **buf, size_t *len, size_t *cap, char c);
static bool sb_pushs(char **buf, size_t *len, size_t *cap, const char *s);

static bool is_var_start(char c) {
  return ((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') || c == '_');
}

static bool is_var_char(char c) {
  return (is_var_start(c) || (c >= '0' && c <= '9'));
}

static char *substr_dup(const char *s, int start, int end) {
  if (end <= start)
    return (ft_strdup(""));
  return (ft_substr(s, (unsigned int)start, (size_t)(end - start)));
}

static bool sb_grow(char **buf, size_t *cap, size_t need) {
  size_t newcap;
  char *nbuf;

  if (*cap >= need)
    return (true);
  newcap = *cap;
  if (newcap == 0)
    newcap = 32;
  while (newcap < need)
    newcap *= 2;
  nbuf = malloc(newcap);
  if (!nbuf)
    return (false);
  if (*buf) {
    memcpy(nbuf, *buf, ft_strlen(*buf) + 1);
    free(*buf);
  }
  *buf = nbuf;
  *cap = newcap;
  return (true);
}

static bool sb_pushc(char **buf, size_t *len, size_t *cap, char c) {
  if (!sb_grow(buf, cap, *len + 2))
    return (false);
  (*buf)[*len] = c;
  (*len)++;
  (*buf)[*len] = '\0';
  return (true);
}

static bool sb_pushs(char **buf, size_t *len, size_t *cap, const char *s) {
  size_t i;

  if (!s)
    return (true);
  i = 0;
  while (s[i]) {
    if (!sb_pushc(buf, len, cap, s[i]))
      return (false);
    i++;
  }
  return (true);
}
char *ms_expand_str(t_ctx *ctx, const char *in, bool in_dquote) {
  t_qstate st;
  size_t len;
  size_t cap;
  int i;
  char *out;
  char *name;
  char *val;

  (void)in_dquote;
  st = Q_NONE;
  out = NULL;
  len = 0;
  cap = 0;
  i = 0;
  if (!sb_grow(&out, &cap, 1))
    return (NULL);
  out[0] = '\0';
  while (in && in[i]) {
    if (st == Q_NONE && in[i] == '\'') {
      st = Q_SINGLE;
      i++;
      continue;
    }
    if (st == Q_NONE && in[i] == '"') {
      st = Q_DOUBLE;
      i++;
      continue;
    }
    if (st == Q_SINGLE && in[i] == '\'') {
      st = Q_NONE;
      i++;
      continue;
    }
    if (st == Q_DOUBLE && in[i] == '"') {
      st = Q_NONE;
      i++;
      continue;
    }
    if (in[i] == '$' && st != Q_SINGLE) {
      i++;
      /* $? → son çıkış kodu */
      if (in[i] == '?') {
        val = ft_itoa(ctx->last_status);
        if (!val || !sb_pushs(&out, &len, &cap, val)) {
          free(val);
          free(out);
          return (NULL);
        }
        free(val);
        i++;
        continue;
      }
      /* $VAR_NAME → environment lookup */
      if (is_var_start(in[i])) {
        int start = i;
        while (in[i] && is_var_char(in[i]))
          i++;
        name = substr_dup(in, start, i);
        if (!name) {
          free(out);
          return (NULL);
        }
        val = ms_env_get(ctx, name);
        free(name);
        if (!sb_pushs(&out, &len, &cap, val)) {
          free(out);
          return (NULL);
        }
        continue;
      }
      /*
      ** $<rakam>: positional parametre yok → rakamı tüket, boş string ver.
      ** Bash davranışı: $1 → "" (minishell'de script modu yok).
      */
      if (in[i] >= '0' && in[i] <= '9') {
        i++; /* rakamı tüket, hiçbir şey basma */
        continue;
      }
      /*
      ** $<geçersiz> veya tek $ → literal '$' bas, karakteri tüketme.
      ** Örnek: "$@" → "$@", "$ " → "$ ", "$" (eol) → "$".
      */
      if (!sb_pushc(&out, &len, &cap, '$')) {
        free(out);
        return (NULL);
      }
      continue;
    }
    if (!sb_pushc(&out, &len, &cap, in[i])) {
      free(out);
      return (NULL);
    }
    i++;
  }
  return (out);
}

bool ms_expand_argv(t_ctx *ctx, char ***argv_io) {
  int i;
  char *new;

  if (!argv_io || !*argv_io)
    return (true);
  i = 0;
  while ((*argv_io)[i]) {
    new = ms_expand_str(ctx, (*argv_io)[i], false);
    if (!new)
      return (false);
    free((*argv_io)[i]);
    (*argv_io)[i] = new;
    i++;
  }
  return (true);
}