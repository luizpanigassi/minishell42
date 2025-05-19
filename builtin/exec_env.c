/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_env.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: luinasci <luinasci@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/21 14:46:59 by luinasci          #+#    #+#             */
/*   Updated: 2025/05/02 16:52:04 by luinasci         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/**
 * @brief Prints environment variables to stdout.
 * @param args Ignored parameter.
 * @return Always returns 0.
 * @note Uses extern environ directly.
 */
int	exec_env(char **args)
{
	extern char	**environ;
	int			i;

	(void)args;
	i = 0;
	while (environ[i])
	{
		if (ft_strncmp(environ[i], "COLUMNS=", 8) != 0
			&& ft_strncmp(environ[i], "LINES=", 6) != 0)
			printf("%s\n", environ[i]);
		i++;
	}
	return (0);
}
