/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_echo.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: luinasci <luinasci@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/17 10:48:11 by jcologne          #+#    #+#             */
/*   Updated: 2025/04/18 15:25:07 by luinasci         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/*
** Prints arguments to stdout with optional newline suppression
** @param args Command arguments (-n flag suppresses newline)
** @return Always returns 0
*/
int	exec_echo(char **args)
{
	int	i;
	int	nl_flag;

	i = 1;
	nl_flag = 0;
	if (args[1] && ft_strcmp(args[1], "-n") == 0)
	{
		nl_flag = 1;
		i++;
	}
	while (args[i])
	{
		ft_putstr_fd(args[i], STDOUT_FILENO);
		if (args[++i])
			ft_putchar_fd(' ', STDOUT_FILENO);
	}
	if (!nl_flag)
		ft_putchar_fd('\n', STDOUT_FILENO);
	return (0);
}
