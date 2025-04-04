/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   signals.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: luinasci <luinasci@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/26 18:28:19 by luinasci          #+#    #+#             */
/*   Updated: 2025/04/02 17:19:03 by luinasci         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/*
** SIGINT handler (Ctrl-C)
** @param sig Signal number
*/
void handle_sigint(int sig)
{
	(void)sig;
	set_exit_status(130); // Set exit status for Ctrl-C
	write(1, "\n", 1);
	rl_replace_line("", 0);
	rl_on_new_line();
	rl_redisplay();
}

/*
** Sets up signal handlers for parent process (shell)
*/
void setup_parent_signals(void)
{
	struct sigaction sa_int;
	struct sigaction sa_quit;

	// Handle SIGINT (Ctrl-C)
	sa_int.sa_handler = handle_sigint;
	sigemptyset(&sa_int.sa_mask);
	sa_int.sa_flags = SA_RESTART;
	sigaction(SIGINT, &sa_int, NULL);

	// Ignore SIGQUIT (Ctrl-\)
	sa_quit.sa_handler = SIG_IGN;
	sigemptyset(&sa_quit.sa_mask);
	sa_quit.sa_flags = 0;
	sigaction(SIGQUIT, &sa_quit, NULL);
}

/*
** Sets up default signal handlers for child processes
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
