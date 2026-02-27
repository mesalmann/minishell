/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hdere <hdere@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/17 13:23:08 by mesalman          #+#    #+#             */
/*   Updated: 2026/02/27 04:28:00 by hdere            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

char **parse(char **argv)
{
    if (!argv || !argv[0])
        return (NULL);
    return (argv);
}

char **lexer(char *line)
{
    if (!line)
        return (NULL);
    return (ft_split(line, ' '));
}

/*static void print_tokens(char **tokens)
{
    int i;

    if (!tokens)
        return;
    i = 0;
    while (tokens[i])
    {
        printf("token[%d] = [%s]\n", i, tokens[i]);
        i++;
    }
}

int main(int ac, char **av)
{
    char    *line;
    char    **arg_list;

    (void)ac;
    (void)av;

    line = readline("minishell> ");
    if (!line)
        return (0);

    arg_list = lexer(line);
    arg_list = parse(arg_list);

    print_tokens(arg_list);

    free_tab(arg_list); // sende zaten ms_find_path içinde kullanıyorsun: free_tab()
    free(line);
    return (0);
}*/