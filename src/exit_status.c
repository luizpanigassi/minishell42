/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exit_status.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: luinasci <luinasci@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/02 17:22:04 by luinasci          #+#    #+#             */
/*   Updated: 2025/04/18 15:24:22 by luinasci         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/**
 * @brief Updates the global exit status variable.
 * @param status Exit code to store (0-255).
 */
void	set_exit_status(int status)
{
	g_exit_status = status;
}

/**
 * @brief Retrieves the global exit status.
 * @return Last exit status of foreground commands.
 */
int	get_exit_status(void)
{
	return (g_exit_status);
}
