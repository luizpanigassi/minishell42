/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_echo.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: luinasci <luinasci@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/17 10:48:11 by jcologne          #+#    #+#             */
/*   Updated: 2025/04/28 19:14:23 by luinasci         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/**
 * @brief Prints arguments to stdout with newline control.
 * @param args Arguments to print (supports -n flag).
 * @return Always returns 0.
 * @note -n flag suppresses trailing newline.
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
