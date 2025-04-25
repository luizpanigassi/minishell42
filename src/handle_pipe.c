/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   handle_pipe.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: luinasci <luinasci@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/21 16:14:25 by luinasci          #+#    #+#             */
/*   Updated: 2025/04/25 16:13:19 by luinasci         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/*
** Executes a pipeline of commands with proper piping
** @param pipeline Command pipeline to execute
** @return Exit status of the last command in pipeline
*/
int execute_pipeline(t_cmd *pipeline)
{
	int prev_pipe[2] = {-1, -1};
	int next_pipe[2] = {-1, -1};
	pid_t *child_pids;
	int cmd_count = ft_cmd_size(pipeline);
	int i = 0;

	if (cmd_count == 0)
		return 1;
	child_pids = malloc(sizeof(pid_t) * cmd_count);
	if (!child_pids)
		return 1;

	t_cmd *current = pipeline;
	while (current)
	{
		t_redir *r = current->redirections;
		while (r)
		{
			if (r->type == T_HEREDOC)
			{
				int hd_fd = create_heredoc(r->filename);
				if (hd_fd < 0)
				{ // Heredoc was interrupted
					free(child_pids);
					free_pipeline(pipeline);
					return 130; // Return SIGINT exit code
				}
				r->fd = hd_fd;
			}
			r = r->next;
		}
		if (current->next && pipe(next_pipe) < 0)
		{
			perror("minishell: pipe");
			free(child_pids);
			return 1;
		}

		pid_t pid = fork();
		if (pid == 0) // Child
		{
			setup_child_signals();
			// Handle pipe redirections
			int pipe_out = -1;
			if (current->next)
				pipe_out = next_pipe[1];
			handle_redirections(prev_pipe[0], pipe_out, current->redirections);

			// Close all pipe ends in child
			if (prev_pipe[0] != -1)
				close(prev_pipe[0]);
			if (prev_pipe[1] != -1)
				close(prev_pipe[1]);
			if (next_pipe[0] != -1)
				close(next_pipe[0]);
			if (next_pipe[1] != -1)
				close(next_pipe[1]);

			if (is_builtin(current->args))
				exit(exec_builtin(current->args));
			else
				exec_external_command(current);
		}
		else if (pid > 0) // Parent
		{
			child_pids[i++] = pid;
			// Close previous pipe ends
			if (prev_pipe[0] != -1)
				close(prev_pipe[0]);
			if (prev_pipe[1] != -1)
				close(prev_pipe[1]);

			// Move to next pipe
			prev_pipe[0] = next_pipe[0];
			prev_pipe[1] = next_pipe[1];
			next_pipe[0] = -1;
			next_pipe[1] = -1;
		}
		else // Fork failed
		{
			perror("minishell: fork");
			free(child_pids);
			return 1;
		}
		current = current->next;
	}

	// Close any remaining pipe ends
	if (prev_pipe[0] != -1)
		close(prev_pipe[0]);
	if (prev_pipe[1] != -1)
		close(prev_pipe[1]);

	// Wait for all children with EINTR handling
	int last_status = 0;
	int child_count = 0;
	while (child_count < cmd_count)
	{
		int status;
		waitpid(child_pids[child_count], &status, 0);
		if (WIFEXITED(status))
			last_status = WEXITSTATUS(status);
		else if (WIFSIGNALED(status))
			last_status = 128 + WTERMSIG(status);
		child_count++;
	}
	// Check if the last command in the pipeline was "exit"
	t_cmd *last_cmd = pipeline;
	while (last_cmd->next)
		last_cmd = last_cmd->next;

	if (is_builtin(last_cmd->args) && ft_strcmp(last_cmd->args[0], "exit") == 0)
	{
		free(child_pids);
		free_pipeline(pipeline);
		exit(last_status); // Exit the shell with the last command's status
	}

	free(child_pids);
	set_exit_status(last_status);
	return last_status;
}
