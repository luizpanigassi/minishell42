/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_exit.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: luinasci <luinasci@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/17 10:43:27 by jcologne          #+#    #+#             */
/*   Updated: 2025/05/08 17:19:43 by luinasci         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/**
 * @brief Terminates the shell with optional status code.
 * @param args Command arguments (optional exit code).
 * @return Special exit code (4242) or error status.
 * @note Handles numeric validation and cleanup before exit.
 */
int	exec_exit(char **args)
{
	int	status;

	status = 0;
	if (args[1] && args[2])
	{
		ft_putstr_fd("minishell: exit: too many arguments\n", STDERR_FILENO);
		set_exit_status(2);
		return (1);
	}
	if (args[1] && !ft_isnumber(args[1]))
	{
		ft_putstr_fd("minishell: exit: numeric argument required\n",
			STDERR_FILENO);
		set_exit_status(2);
		exit(2);
	}
	if (args[1])
		status = ft_atoi(args[1]) % 256;
	ft_putstr_fd("Exiting minishell, goodbye!\n", STDOUT_FILENO);
	rl_clear_history();
	exit(status);
}
