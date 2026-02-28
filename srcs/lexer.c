<<<<<<< HEAD
#include "minishell.h"

static char *read_word(const char *s, int *i);

void ms_token_free(t_token *toks)
{
    t_token *tmp;

    while (toks)
    {
        tmp = toks->next;
        if (toks->lex)
            free(toks->lex);
        free(toks);
        toks = tmp;
    }
}
//operatör mü kontrolü yapılır
int	is_operator(char c)
{
	return (c == '<' || c == '>' || c == '|');
}

//boşlukları geçer
void	skip_spaces(const char *line, int *i)
{
	while (line[*i] && ft_isspace(line[*i]))
		(*i)++;
}


t_token *handle_operator(const char *line, int *i)
{
    t_opkind op;
    char     c;

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

t_token *handle_word(const char *line, int *i)
{
    t_token *new;
    char    *lex;

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

static int  ms_is_end(char c)
{
    return (ft_isspace(c) || is_operator(c));
}

static int  out_len(const char *s, int i)
{
    t_qstate st;
    int      n;

    st = Q_NONE;
    n = 0;
    while (s[i])
    {
        if (st == Q_NONE)
        {
            if (ms_is_end(s[i]))
                break;
            if (s[i] == '\'') { st = Q_SINGLE; i++; continue; }
            if (s[i] == '"')  { st = Q_DOUBLE; i++; continue; }
            n++; i++;
        }
        else if (st == Q_SINGLE)
        {
            if (s[i] == '\'') { st = Q_NONE; i++; continue; }
            n++; i++;
        }
        else // Q_DOUBLE
        {
            if (s[i] == '"') { st = Q_NONE; i++; continue; }
            n++; i++;
        }
    }
    if (st != Q_NONE)
        return (-1); // unclosed quote
    return (n);
}

static char *read_word(const char *s, int *i)
{
    t_qstate st;
    char     *out;
    int      n;
    int      k;

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
                break;
            if (s[*i] == '\'') { st = Q_SINGLE; (*i)++; continue; }
            if (s[*i] == '"')  { st = Q_DOUBLE; (*i)++; continue; }
            out[k++] = s[*i];
            (*i)++;
        }
        else if (st == Q_SINGLE)
        {
            if (s[*i] == '\'') { st = Q_NONE; (*i)++; continue; }
            out[k++] = s[*i];
            (*i)++;
        }
        else // Q_DOUBLE
        {
            if (s[*i] == '"') { st = Q_NONE; (*i)++; continue; }
            out[k++] = s[*i];
            (*i)++;
        }
    }
    out[k] = '\0';
    return (out);
}

t_token	*lexer(const char *line)
{
	t_token	*head;
	t_token *tail;
	t_token	*new;
	int		i;

	head = NULL;
	tail = NULL;
	i = 0;
	while (line && line[i])
	{
		skip_spaces(line, &i);
        //command satırındaki boşlukları geç
        // örn "     cat a.txt"
		if (!line[i])
        //komut yoksa döngüyü kır
        //örn " "
			break ;
		if (is_operator(line[i]))
			new = handle_operator(line, &i);
            //operatörle karşılaşırsan sturctta tut
		else
			new = handle_word(line, &i);
            //operatör değilse kelimedir
		if (!new) 
		{
    		m
            s_token_free(head);
    		return (NULL);
		}
		if (head == NULL)
		{
			head = new;
			tail = new;
		}
		else
		{
			tail->next = new;
			tail = new;
		}
        //token listesine ekle
	}
	return (head);
=======
/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_split.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mesalman <mesalman@student.42istanbul.com  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/17 13:23:08 by mesalman          #+#    #+#             */
/*   Updated: 2025/06/26 15:18:16 by mesalman         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

char **parse(char **argv)
{
    if (!argv)
        return (NULL);
    if (!argv[0])
        return (NULL);
    return (argv);
}

char **lexer(char *line)
{
    char ** arg_list;
    if(!line)
        return 0;
    return(ft_split(line));
}

int main(int ac, char **av)
{
    char **arg_list;
    char *line;
    arg_list = lexer(line);
    parse(arg_list);
>>>>>>> executer
}