/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipe_signals.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: luinasci <luinasci@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/08 16:30:16 by luinasci          #+#    #+#             */
/*   Updated: 2025/05/08 16:31:07 by luinasci         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/**
 * @brief Sets up signal handling for the pipeline execution.
 * @param sa Pointer to the new sigaction structure.
 * @param old_sa Pointer to the old sigaction structure to restore later.
 * @return 0 on success, 1 on failure.
 */
int	setup_signal_handling(struct sigaction *sa, struct sigaction *old_sa)
{
	sa->sa_handler = SIG_IGN;
	sa->sa_flags = SA_RESTART;
	sigemptyset(&sa->sa_mask);
	if (sigaction(SIGINT, sa, old_sa) == -1)
	{
		perror("minishell: sigaction");
		return (1);
	}
	return (0);
}

/**
 * @brief Sets up and restores signal handling for pipeline execution.
 * @param old_sa Pointer to the old sigaction structure to restore later.
 * @param was_signaled Pointer to a flag indicating if a signal was received.
 * @return 0 on success, 1 on failure.
 */
int	manage_signal_handling(struct sigaction *old_sa, int was_signaled)
{
	if (was_signaled)
		write(STDOUT_FILENO, "\n", 1);
	if (sigaction(SIGINT, old_sa, NULL) == -1)
	{
		perror("minishell: sigaction");
		return (1);
	}
	return (0);
}

/**
 * @brief Creates a pipe and handles errors.
 * @param next_pipe Array representing the next pipe.
 * @param child_pids Array of child process IDs to free in case of error.
 * @return 0 on success, 1 on failure.
 */
int	create_pipe(int next_pipe[2], pid_t *child_pids)
{
	if (pipe(next_pipe) < 0)
	{
		perror("minishell: pipe");
		free(child_pids);
		return (1);
	}
	return (0);
}
