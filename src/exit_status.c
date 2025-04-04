/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exit_status.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: luinasci <luinasci@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/02 17:22:04 by luinasci          #+#    #+#             */
/*   Updated: 2025/04/02 17:22:10 by luinasci         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/*
** Sets the global exit status
** @param status Exit status to set
*/
void set_exit_status(int status)
{
	g_exit_status = status;
}

/*
** Gets the current global exit status
** @return Current exit status
*/
int get_exit_status(void)
{
	return (g_exit_status);
}
