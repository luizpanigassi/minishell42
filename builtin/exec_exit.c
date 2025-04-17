/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_exit.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jcologne <jcologne@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/17 10:43:27 by jcologne          #+#    #+#             */
/*   Updated: 2025/04/17 11:29:56 by jcologne         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/*
** Exits the shell with optional status code
** @param args Command arguments (args[1] is optional exit status)
** @return Does not return on success, returns 1 if too many arguments
*/
int	exec_exit(char **args)
{
	int	status;

	status = 0;
	if (args[1] && args[2])
	{
		ft_putstr_fd("minishell: exit error: too many arguments\n", STDERR_FILENO);
		return (1);
	}
	if (args[1])
	{
		if (ft_isnumber(args[1]))
			status = ft_atoi(args[1]);
		else
		{
			ft_putstr_fd("minishell: exit error: numeric argument required\n", STDERR_FILENO);
			return (2);
		}
	}
	printf("Exiting minishell, goodbye!\n");
	rl_clear_history();
	exit(status);
}
