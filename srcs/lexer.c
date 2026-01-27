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
}