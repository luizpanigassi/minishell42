/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   create_heredoc.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: luinasci <luinasci@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/17 09:59:44 by jcologne          #+#    #+#             */
/*   Updated: 2025/05/08 16:44:09 by luinasci         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <minishell.h>

/**
 * @brief Sets up and restores signal handling for heredoc creation.
 * @param sa Pointer to the new sigaction structure.
 * @param old_sa Pointer to the old sigaction structure to restore later.
 * @return 0 on success, -1 on failure.
 */
int	setup_and_restore_signals(struct sigaction *sa, struct sigaction *old_sa)
{
	sa->sa_handler = SIG_IGN;
	sa->sa_flags = SA_RESTART;
	sigemptyset(&sa->sa_mask);
	if (sigaction(SIGINT, sa, old_sa) == -1)
		return (-1);
	return (0);
}

/**
 * @brief Sets up a pipe and forks a child process.
 * @param pipefd Array to store the pipe file descriptors.
 * @param old_sa Pointer to the old sigaction structure to restore later.
 * @return The PID of the child process on success, -1 on failure.
 */
pid_t	setup_pipe_and_fork(int pipefd[2], struct sigaction *old_sa)
{
	pid_t	pid;

	if (pipe(pipefd) < 0)
		return (-1);
	pid = fork();
	if (pid == -1)
	{
		close(pipefd[0]);
		close(pipefd[1]);
		sigaction(SIGINT, old_sa, NULL);
	}
	return (pid);
}

/**
 * @brief Handles the child process setup for heredoc creation.
 * @param write_fd File descriptor for the write end of the pipe.
 * @param delimiter The delimiter string to terminate the heredoc.
 * @note Sets up signals and invokes the child process logic.
 */
void	setup_child_process(int write_fd, const char *delimiter)
{
	close(write_fd - 1);
	setup_heredoc_signals();
	handle_child_process(write_fd, delimiter);
}

/**
 * @brief Creates a heredoc by forking a child process.
 * @param delimiter The delimiter string to terminate the heredoc.
 * @return File descriptor for the read end of the pipe on success,
 * -1 on failure.
 * @note The child process handles user input, while the parent
 * waits for completion.
 */
int	create_heredoc(const char *delimiter)
{
	t_heredoc_context	ctx;
	pid_t				pid;
	int					result;

	if (setup_and_restore_signals(&ctx.sa, &ctx.old_sa) == -1)
		return (-1);
	pid = setup_pipe_and_fork(ctx.pipefd, &ctx.old_sa);
	if (pid == -1)
		return (-1);
	if (pid == 0)
	{
		setup_child_process(ctx.pipefd[1], delimiter);
	}
	else
	{
		close(ctx.pipefd[1]);
		result = handle_parent_process(pid, ctx.pipefd[0], &ctx.was_signaled);
		if (ctx.was_signaled)
			write(STDOUT_FILENO, "\n", 1);
		sigaction(SIGINT, &ctx.old_sa, NULL);
		return (result);
	}
	return (0);
}
