/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   handle_pipe.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: luinasci <luinasci@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/21 16:14:25 by luinasci          #+#    #+#             */
/*   Updated: 2025/05/13 15:25:56 by luinasci         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/**
 * @brief Waits for all child processes and determines the last exit status.
 * @param child_pids Array of child process IDs.
 * @param cmd_count Number of commands (child processes).
 * @return Exit status of the last command.
 */
int	wait_for_children(pid_t *child_pids, int cmd_count, int *was_signaled)
{
	int	last_status;
	int	child_count;
	int	status;

	if (!child_pids)
		return (0);
	child_count = 0;
	last_status = 0;
	*was_signaled = 0;
	while (child_count < cmd_count)
	{
		waitpid(child_pids[child_count], &status, 0);
		if (WIFEXITED(status))
			last_status = WEXITSTATUS(status);
		else if (WIFSIGNALED(status))
		{
			last_status = 128 + WTERMSIG(status);
			if (WTERMSIG(status) == SIGINT)
				*was_signaled = 1;
		}
		child_count++;
	}
	return (last_status);
}

/**
 * @brief Closes the previous pipe and updates the pipe arrays.
 * @param prev_pipe Array representing the previous pipe.
 * @param next_pipe Array representing the next pipe.
 * @note Ensures proper cleanup and preparation for the next command
 * in the pipeline.
 */
void	close_and_update_pipes(int prev_pipe[2], int next_pipe[2])
{
	if (prev_pipe[0] != -1)
		close(prev_pipe[0]);
	if (prev_pipe[1] != -1)
		close(prev_pipe[1]);
	prev_pipe[0] = next_pipe[0];
	prev_pipe[1] = next_pipe[1];
	next_pipe[0] = -1;
	next_pipe[1] = -1;
}

/**
 * @brief Closes any remaining open pipes.
 * @param prev_pipe Array representing the previous pipe.
 * @note Prevents resource leaks by closing unused file descriptors.
 */
void	close_remaining_pipes(int prev_pipe[2])
{
	if (prev_pipe[0] != -1)
		close(prev_pipe[0]);
	if (prev_pipe[1] != -1)
		close(prev_pipe[1]);
}

/**
 * @brief Handles heredoc redirections for a command.
 * @param redirections Linked list of redirections for the command.
 * @return 0 on success, 130 if heredoc was interrupted.
 */
int	handle_heredoc_redirections(t_redir *redirections)
{
	t_redir	*r;
	int		hd_fd;

	r = redirections;
	while (r)
	{
		if (r->type == T_HEREDOC)
		{
			hd_fd = create_heredoc(r->filename);
			if (hd_fd < 0)
			{
				return (130);
			}
			r->fd = hd_fd;
		}
		r = r->next;
	}
	return (0);
}

/**
 * @brief Handles the execution of the last command in the pipeline.
 * @param pipeline Linked list of commands to execute.
 * @param last_status Exit status of the last command.
 * @param child_pids Array of child process IDs.
 */
void	handle_last_command(t_cmd *pipeline, int last_status, pid_t *child_pids)
{
	t_cmd	*last_cmd;

	last_cmd = pipeline;
	while (last_cmd->next)
		last_cmd = last_cmd->next;
	if (is_builtin(last_cmd->args) && ft_strcmp(last_cmd->args[0], "exit") == 0)
	{
		free(child_pids);
		free_pipeline(pipeline);
		exit(last_status);
	}
}
