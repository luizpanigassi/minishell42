/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   signals.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: luinasci <luinasci@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/26 18:28:19 by luinasci          #+#    #+#             */
/*   Updated: 2025/04/07 18:04:31 by luinasci         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/**
 * @brief Handles SIGINT (Ctrl+C) in interactive mode.
 * Prints a new prompt and resets input buffer.
 * @param sig Signal number (unused).
 */
void handle_sigint(int sig)
{
	(void)sig;
	write(STDOUT_FILENO, "\n", 1);
	rl_replace_line("", 0); // Reset readline buffer
	rl_on_new_line();
	rl_redisplay();
	set_exit_status(130); // Set $? to 130 (SIGINT code)
}

/*
** Sets up signal handlers for parent process (shell)
*/
void setup_parent_signals(void)
{
	struct sigaction sa;

	sa.sa_handler = handle_sigint; // Custom SIGINT handler
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = SA_RESTART;
	sigaction(SIGINT, &sa, NULL);

	sa.sa_handler = SIG_IGN; // Ignore SIGQUIT in parent
	sigaction(SIGQUIT, &sa, NULL);
}

/**
 * @brief Configures child processes to use default signal handlers.
 * SIGINT and SIGQUIT will terminate children normally.
 */
void setup_child_signals(void)
{
	struct sigaction sa_default;

	sa_default.sa_handler = SIG_DFL;
	sigemptyset(&sa_default.sa_mask);
	sa_default.sa_flags = 0;

	// Reset to default signals
	signal(SIGINT, SIG_DFL);
	signal(SIGQUIT, SIG_DFL);
}
