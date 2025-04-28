/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_pwd.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: luinasci <luinasci@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/17 10:52:50 by jcologne          #+#    #+#             */
/*   Updated: 2025/04/28 19:15:45 by luinasci         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/**
 * @brief Prints current working directory.
 * @param args Ignored parameter.
 * @return 0 on success, 1 on error.
 * @note Uses getcwd() system call.
 */
int	exec_pwd(char **args)
{
	char	*cwd;

	(void)args;
	cwd = getcwd(NULL, 0);
	if (args[1])
	{
		ft_putstr_fd("minishell: pwd: too many arguments\n", STDERR_FILENO);
		free(cwd);
		return (1);
	}
	if (cwd && args[1] == NULL)
	{
		printf("%s\n", cwd);
		free(cwd);
		return (0);
	}
	else
	{
		perror("minishell: pwd");
		return (1);
	}
}
