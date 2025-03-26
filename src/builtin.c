/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jcologne <jcologne@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/12 18:24:27 by jcologne          #+#    #+#             */
/*   Updated: 2025/03/13 15:52:15 by jcologne         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	is_builtin(char **args)
{
	if (ft_strncmp(args[0], "cd", 3) == 0)
		return (exec_cd(args));
	if (ft_strncmp(args[0], "echo", 5) == 0)
		return (exec_echo(args));
	/*if (ft_strncmp(args[0], "pwd") == 0)
		return (exec_pwd(args));
	if (ft_strncmp(args[0], "export") == 0)
		return (exec_export(args));
	if (ft_strncmp(args[0], "unset") == 0)
		return (exec_unset(args));
	if (ft_strncmp(args[0], "env") == 0)
		return (exec_env(args));*/
	if (ft_strncmp(args[0], "exit", 5) == 0)
		return (exec_exit(args));
	return (0);
}

int exec_cd(char **args)
{
	printf("Executar cd_%s\n", args[0]);
	return (1);
}

int	exec_exit(char **args)
{
	printf("%s\n", args[0]);
	exit(0);
	return (1);
}

int exec_echo(char **args)
{	int	i;

	i = 1;
	while (args[i])
		printf("%s", args[i++]);
	printf("\n");
	return (1);
}