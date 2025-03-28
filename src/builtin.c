/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: luinasci <luinasci@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/26 18:05:42 by luinasci          #+#    #+#             */
/*   Updated: 2025/03/28 18:25:26 by luinasci         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int is_builtin(char **tokens)
{
	if (!tokens || !tokens[0])
		return (0);
	if (ft_strncmp(tokens[0], "cd", 3) == 0)
		return (1);
	if (ft_strncmp(tokens[0], "echo", 5) == 0)
		return (1);
	/*if (ft_strncmp(tokens[0], "pwd") == 0)
		return (exec_pwd(tokens));
	if (ft_strncmp(tokens[0], "export") == 0)
		return (exec_export(tokens));
	if (ft_strncmp(tokens[0], "unset") == 0)
		return (exec_unset(args));
	if (ft_strncmp(tokens[0], "env") == 0)
		return (exec_env(tokens));*/
	if (ft_strncmp(tokens[0], "exit", 5) == 0)
		return (1);
	return (0);
}

int exec_builtin(char **args)
{
	if (ft_strncmp(args[0], "cd", 3) == 0)
		return (exec_cd(args));
	if (ft_strncmp(args[0], "echo", 5) == 0)
		return (exec_echo(args));
	if (ft_strncmp(args[0], "exit", 5) == 0)
		return (exec_exit(args));
	return (0);
}

int exec_cd(char **args)
{
	printf("Executar cd_%s\n", args[0]);
	return (1);
}

int exec_exit(char **args)
{
	(void)args;
	printf("Exiting minishell, goodbye!\n");
	return (2);
}

int exec_echo(char **args)
{
	int i;

	i = 1;
	while (args[i])
		printf("%s", args[i++]);
	printf("\n");
	return (1);
}
