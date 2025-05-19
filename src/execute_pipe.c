/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execute_pipe.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: luinasci <luinasci@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/06 16:56:22 by luinasci          #+#    #+#             */
/*   Updated: 2025/05/13 15:27:08 by luinasci         ###   ########.fr       */
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
 * @brief Forks a process and handles parent/child behavior.
 * @param prev_pipe Array representing the previous pipe.
 * @param next_pipe Array representing the next pipe.
 * @param current Current command in the pipeline.
 * @param child_pids Array of child process IDs.
 * @param i Index for storing the child PID.
 * @return 0 on success, 1 on failure.
 */
int	fork_and_execute(int *pipes[2], t_cmd *current, pid_t *child_pids, int *i)
{
	pid_t	pid;

	pid = fork();
	if (pid == 0)
	{
		execute_child_process(pipes[0], pipes[1], current);
	}
	else if (pid > 0)
	{
		child_pids[(*i)++] = pid;
		close_and_update_pipes(pipes[0], pipes[1]);
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
 * @brief Processes a single command in the pipeline.
 * @param ctx Pipeline context containing state and resources.
 * @param pipes Array of previous and next pipes.
 * @return 0 on success, non-zero exit code on failure.
 */
int	process_pipeline_command(t_pipeline_context *ctx, int *pipes[2])
{
	if (handle_heredoc_redirections(ctx->current->redirections) == 130)
		return (cleanup_on_failure(&ctx->child_pids, 130));
	if (ctx->current->next && create_pipe(ctx->next_pipe, ctx->child_pids))
		return (cleanup_on_failure(&ctx->child_pids, 1));
	if (fork_and_execute(pipes, ctx->current, ctx->child_pids, &ctx->index))
		return (cleanup_on_failure(&ctx->child_pids, 1));
	return (0);
}

/**
 * @brief Processes all commands in the pipeline.
 * @param ctx Pipeline context containing state and resources.
 * @param pipes Array of previous and next pipes.
 * @param old_sa Pointer to the old sigaction structure to restore later.
 * @return 0 on success, 1 on failure.
 */
int	process_all_pipeline_commands(t_pipeline_context *ctx,
	int *pipes[2], struct sigaction *old_sa)
{
	while (ctx->current)
	{
		if (process_pipeline_command(ctx, pipes))
		{
			sigaction(SIGINT, old_sa, NULL);
			return (1);
		}
		ctx->current = ctx->current->next;
	}
	return (0);
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
	int					*pipes[2];
	struct sigaction	sa;
	struct sigaction	old_sa;
	int					was_signaled;

	if (setup_signal_handling(&sa, &old_sa))
		return (cleanup_on_failure(NULL, 1));
	pipes[0] = ctx.prev_pipe;
	pipes[1] = ctx.next_pipe;
	if (initialize_pipeline(pipeline, &ctx, &old_sa))
		return (cleanup_on_failure(&ctx.child_pids, 1));
	if (process_all_pipeline_commands(&ctx, pipes, &old_sa))
		return (cleanup_on_failure(&ctx.child_pids, 1));
	close_remaining_pipes(ctx.prev_pipe);
	ctx.last_status = wait_for_children(ctx.child_pids,
			ft_cmd_size(pipeline), &was_signaled);
	handle_last_command(pipeline, ctx.last_status, ctx.child_pids);
	free(ctx.child_pids);
	if (manage_signal_handling(&old_sa, was_signaled))
		return (1);
	set_exit_status(ctx.last_status);
	return (ctx.last_status);
}
