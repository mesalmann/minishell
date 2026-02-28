#include "minishell.h"

t_token	*token_new(t_tokkind kind, t_opkind op, char *lex)
{
	t_token	*tok;

	tok = malloc(sizeof(t_token));
	if (!tok)
		return (NULL);
	tok->kind = kind;
	tok->op = op;
	tok->lex = lex;
	tok->next = NULL;
	return (tok);
}
#include "minishell.h"
#include <ctype.h>

static char		*read_word(const char *s, int *i);

void	ms_token_free(t_token *toks)
{
	t_token	*tmp;

	while (toks)
	{
		tmp = toks->next;
		if (toks->lex)
			free(toks->lex);
		free(toks);
		toks = tmp;
	}
}

/* Operatör mü kontrolü */
int	is_operator(char c)
{
	return (c == '<' || c == '>' || c == '|');
}

/* Boşlukları geç */
void	skip_spaces(const char *line, int *i)
{
	while (line[*i] && isspace(line[*i]))
		(*i)++;
}

t_token	*handle_operator(const char *line, int *i)
{
	t_opkind	op;
	char		c;

	op = OP_NONE;
	c = line[*i];
	if (c == '<')
	{
		if (line[*i + 1] == '<')
		{
			op = OP_HEREDOC;
			*i += 2;
		}
		else
		{
			op = OP_IN;
			*i += 1;
		}
	}
	else if (c == '>')
	{
		if (line[*i + 1] == '>')
		{
			op = OP_OUT_APPEND;
			*i += 2;
		}
		else
		{
			op = OP_OUT_TRUNC;
			*i += 1;
		}
	}
	else if (c == '|')
	{
		op = OP_PIPE;
		*i += 1;
	}
	else
		return (NULL);
	return (token_new(TK_OP, op, NULL));
}

t_token	*handle_word(const char *line, int *i)
{
	t_token	*new;
	char	*lex;

	lex = read_word(line, i);
	if (!lex)
		return (NULL);
	new = token_new(TK_WORD, OP_NONE, lex);
	if (!new)
	{
		free(lex);
		return (NULL);
	}
	return (new);
}

static int	ms_is_end(char c)
{
	return (isspace(c) || is_operator(c));
}

static int	out_len(const char *s, int i)
{
	t_qstate	st;
	int			n;

	st = Q_NONE;
	n = 0;
	while (s[i])
	{
		if (st == Q_NONE)
		{
			if (ms_is_end(s[i]))
				break ;
			if (s[i] == '\'') { st = Q_SINGLE; i++; continue ; }
			if (s[i] == '"')  { st = Q_DOUBLE; i++; continue ; }
			n++;
			i++;
		}
		else if (st == Q_SINGLE)
		{
			if (s[i] == '\'') { st = Q_NONE; i++; continue ; }
			n++;
			i++;
		}
		else /* Q_DOUBLE */
		{
			if (s[i] == '"') { st = Q_NONE; i++; continue ; }
			n++;
			i++;
		}
	}
	if (st != Q_NONE)
		return (-1); /* unclosed quote */
	return (n);
}

static char	*read_word(const char *s, int *i)
{
	t_qstate	st;
	char		*out;
	int			n;
	int			k;

	n = out_len(s, *i);
	if (n < 0)
		return (NULL);
	out = malloc((size_t)n + 1);
	if (!out)
		return (NULL);
	st = Q_NONE;
	k = 0;
	while (s[*i])
	{
		if (st == Q_NONE)
		{
			if (ms_is_end(s[*i]))
				break ;
			if (s[*i] == '\'') { st = Q_SINGLE; (*i)++; continue ; }
			if (s[*i] == '"')  { st = Q_DOUBLE; (*i)++; continue ; }
			out[k++] = s[(*i)++];
		}
		else if (st == Q_SINGLE)
		{
			if (s[*i] == '\'') { st = Q_NONE; (*i)++; continue ; }
			out[k++] = s[(*i)++];
		}
		else /* Q_DOUBLE */
		{
			if (s[*i] == '"') { st = Q_NONE; (*i)++; continue ; }
			out[k++] = s[(*i)++];
		}
	}
	out[k] = '\0';
	return (out);
}

t_token	*lexer(const char *line)
{
	t_token	*head;
	t_token	*tail;
	t_token	*new;
	int		i;

	head = NULL;
	tail = NULL;
	i = 0;
	while (line && line[i])
	{
		skip_spaces(line, &i);
		if (!line[i])
			break ;
		if (is_operator(line[i]))
			new = handle_operator(line, &i);
		else
			new = handle_word(line, &i);
		if (!new)
		{
			ms_token_free(head);
			return (NULL);
		}
		if (!head)
		{
			head = new;
			tail = new;
		}
		else
		{
			tail->next = new;
			tail = new;
		}
	}
	return (head);
}