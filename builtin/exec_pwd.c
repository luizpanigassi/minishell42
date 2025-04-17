/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_pwd.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jcologne <jcologne@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/17 10:52:50 by jcologne          #+#    #+#             */
/*   Updated: 2025/04/17 11:30:38 by jcologne         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/*
** Prints the current working directory
** @param args Ignored
** @return 0 on success, 1 on failure
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
