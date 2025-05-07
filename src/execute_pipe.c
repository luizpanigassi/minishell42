/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execute_pipe.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: luinasci <luinasci@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/06 16:56:22 by luinasci          #+#    #+#             */
/*   Updated: 2025/05/07 16:52:57 by luinasci         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/**
 * @brief Executes the logic for a child process in the pipeline.
 * @param prev_pipe Array representing the previous pipe.
 * @param next_pipe Array representing the next pipe.
 * @param current Current command in the pipeline.
 */
void	execute_child_process(int prev_pipe[2],
	int next_pipe[2], t_cmd *current)
{
	int	pipe_out;

	setup_child_signals();
	pipe_out = -1;
	if (current->next)
		pipe_out = next_pipe[1];
	handle_redirections(prev_pipe[0], pipe_out, current->redirections);
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

/**
 * @brief Forks a process and handles parent/child behavior.
 * @param prev_pipe Array representing the previous pipe.
 * @param next_pipe Array representing the next pipe.
 * @param current Current command in the pipeline.
 * @param child_pids Array of child process IDs.
 * @param i Index for storing the child PID.
 * @return 0 on success, 1 on failure.
 */
int	fork_and_execute(int prev_pipe[2], int next_pipe[2],
	t_cmd *current, pid_t *child_pids, int *i)
{
	pid_t	pid;

	pid = fork();
	if (pid == 0)
		execute_child_process(prev_pipe, next_pipe, current);
	else if (pid > 0)
	{
		child_pids[(*i)++] = pid;
		close_and_update_pipes(prev_pipe, next_pipe);
	}
	else
	{
		perror("minishell: fork");
		free(child_pids);
		return (1);
	}
	return (0);
}

/**
 * @brief Cleans up resources and handles errors during pipeline execution.
 * @param child_pids Array of child process IDs.
 * @param pipeline Linked list of commands to execute.
 * @param exit_code Exit code to return after cleanup.
 * @return The provided exit code.
 */
int	cleanup_on_failure(pid_t *child_pids, t_cmd *pipeline, int exit_code)
{
	free(child_pids);
	free_pipeline(pipeline);
	return (exit_code);
}

/**
 * @brief Manages pipeline execution with process forking.
 * @param pipeline Linked list of commands to execute.
 * @return Exit status of last command in pipeline.
 * @note Handles input/output redirection between commands.
 */
int	execute_pipeline(t_cmd *pipeline)
{
	t_pipeline_context	ctx;

	if (initialize_pipeline_resources(pipeline, ctx.prev_pipe,
			ctx.next_pipe, &ctx.child_pids))
		return (cleanup_on_failure(ctx.child_pids, pipeline, 1));
	ctx.index = 0;
	ctx.current = pipeline;
	while (ctx.current)
	{
		if (handle_heredoc_redirections(ctx.current->redirections) == 130)
			return (cleanup_on_failure(ctx.child_pids, pipeline, 130));
		if (ctx.current->next && create_pipe(ctx.next_pipe, ctx.child_pids))
			return (cleanup_on_failure(ctx.child_pids, pipeline, 1));
		if (fork_and_execute(ctx.prev_pipe, ctx.next_pipe, ctx.current,
				ctx.child_pids, &ctx.index))
			return (cleanup_on_failure(ctx.child_pids, pipeline, 1));
		ctx.current = ctx.current->next;
	}
	close_remaining_pipes(ctx.prev_pipe);
	ctx.last_status = wait_for_children(ctx.child_pids, ft_cmd_size(pipeline));
	handle_last_command(pipeline, ctx.last_status, ctx.child_pids);
	free(ctx.child_pids);
	set_exit_status(ctx.last_status);
	return (ctx.last_status);
}
