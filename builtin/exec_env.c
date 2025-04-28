/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_env.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: luinasci <luinasci@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/21 14:46:59 by luinasci          #+#    #+#             */
/*   Updated: 2025/04/28 19:14:40 by luinasci         ###   ########.fr       */
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
	i = -1;
	while (environ[++i])
		printf("%s\n", environ[i]);
	return (0);
}
