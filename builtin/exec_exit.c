/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_exit.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: luinasci <luinasci@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/17 10:43:27 by jcologne          #+#    #+#             */
/*   Updated: 2025/04/28 19:14:54 by luinasci         ###   ########.fr       */
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
		ft_putstr_fd("minishell: exit error: too many arguments\n",
			STDERR_FILENO);
		set_exit_status(1);
		return (1);
	}
	if (args[1])
	{
		if (ft_isnumber(args[1]))
			status = ft_atoi(args[1]) % 256;
		else
		{
			ft_putstr_fd("minishell: exit error: numeric argument required\n",
				STDERR_FILENO);
			return (2);
		}
	}
	printf("Exiting minishell, goodbye!\n");
	rl_clear_history();
	exit(status);
}
