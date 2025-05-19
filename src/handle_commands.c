/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   handle_commands.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: luinasci <luinasci@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/07 17:41:53 by luinasci          #+#    #+#             */
/*   Updated: 2025/05/08 16:48:11 by luinasci         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/**
 * @brief Executes a non-builtin command.
 * @param cmd Command structure to execute.
 * @param environ Environment variables.
 */
void	execute_non_builtin(t_cmd *cmd, char **environ)
{
	char	*cmd_path;

	cmd_path = get_cmd_path(cmd->args[0]);
	if (!cmd_path)
	{
		ft_putstr_fd("minishell: command not found: ", STDERR_FILENO);
		ft_putstr_fd(cmd->args[0], STDERR_FILENO);
		ft_putstr_fd("\n", STDERR_FILENO);
		exit(CMD_NOT_FOUND);
	}
	execve(cmd_path, cmd->args, environ);
	perror("minishell");
	free(cmd_path);
	exit(EXIT_FAILURE);
}

/**
 * @brief Restores the SIGINT handler and handles the child process's
 * exit status.
 * @param pid Process ID of the child.
 * @param old_sa Previous SIGINT signal action to restore.
 */
void	handle_child_exit(pid_t pid, struct sigaction *old_sa)
{
	int	status;
	int	was_signaled;

	was_signaled = 0;
	waitpid(pid, &status, 0);
	if (WIFEXITED(status))
		set_exit_status(WEXITSTATUS(status));
	else if (WIFSIGNALED(status))
	{
		set_exit_status(128 + WTERMSIG(status));
		if (WTERMSIG(status) == SIGINT)
			was_signaled = 1;
	}
	if (was_signaled)
		write(STDOUT_FILENO, "\n", 1);
	if (sigaction(SIGINT, old_sa, NULL) == -1)
		perror("minishell: sigaction");
}

/**
 * @brief Sets up the child process for execution.
 * @param cmd Command structure to execute.
 * @param pipe_in Input file descriptor (or -1).
 * @param pipe_out Output file descriptor (or -1).
 */
void	setup_and_execute_child(t_cmd *cmd, int pipe_in, int pipe_out)
{
	extern char	**environ;

	setup_child_signals();
	handle_redirections(pipe_in, pipe_out, cmd->redirections);
	if (is_builtin(cmd->args))
	{
		exec_builtin(cmd->args);
		exit(g_exit_status);
	}
	else
	{
		execute_non_builtin(cmd, environ);
	}
}

/**
 * @brief Handles built-in commands in the parent process.
 * @param pipeline The command pipeline structure.
 */
void	handle_builtin_in_parent(t_cmd *pipeline)
{
	int	saved_stdin;
	int	saved_stdout;
	int	saved_stderr;
	int	exit_code;

	saved_stdin = dup(STDIN_FILENO);
	saved_stdout = dup(STDOUT_FILENO);
	saved_stderr = dup(STDERR_FILENO);
	if (handle_redirections(-1, -1, pipeline->redirections) == 0)
	{
		exit_code = exec_builtin(pipeline->args);
		if (exit_code != 4242)
			g_exit_status = exit_code;
	}
	dup2(saved_stdin, STDIN_FILENO);
	dup2(saved_stdout, STDOUT_FILENO);
	dup2(saved_stderr, STDERR_FILENO);
	close(saved_stdin);
	close(saved_stdout);
	close(saved_stderr);
}

/**
 * @brief Handles a single command pipeline, including parsing, execution,
 * and cleanup.
 * @param command The command string to process.
 * @param should_exit Pointer to the exit flag.
 * @return 1 if a syntax error occurred, 0 otherwise.
 */
int	handle_command_pipeline(char *command, int *should_exit)
{
	t_cmd	*pipeline;
	t_parse	parser;
	int		pipeline_status;

	pipeline = parse_and_validate_pipeline(command, &parser);
	if (!pipeline)
	{
		if (parser.syntax_error)
			return (1);
		else
			return (0);
	}
	if (!pipeline->next && is_builtin(pipeline->args))
		handle_builtin_in_parent(pipeline);
	else
	{
		pipeline_status = execute_pipeline(pipeline);
		if (pipeline_status == EXIT_CODE_EXIT)
		{
			*should_exit = 1;
			set_exit_status(get_exit_status());
		}
	}
	free_pipeline(pipeline);
	return (0);
}
