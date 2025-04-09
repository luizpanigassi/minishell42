/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: luinasci <luinasci@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/11 15:43:31 by jcologne          #+#    #+#             */
/*   Updated: 2025/04/09 15:15:58 by luinasci         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

volatile sig_atomic_t g_exit_status = 0;

/**
 * @brief Frees a command pipeline and associated resources.
 * @param pipeline Linked list of command structures.
 */
void free_pipeline(t_cmd *pipeline)
{
	t_cmd *current;
	t_redir *redir, *tmp_redir;

	while (pipeline)
	{
		current = pipeline;
		pipeline = pipeline->next;

		// Free arguments
		if (current->args)
			ft_free_array(current->args);

		// Free redirections
		redir = current->redirections;
		while (redir)
		{
			tmp_redir = redir;
			redir = redir->next;
			free(tmp_redir->filename);
			free(tmp_redir);
		}

		free(current);
	}
}

/**
 * @brief Executes a single command with I/O redirection.
 * @param cmd Command structure to execute.
 * @param pipe_in Input file descriptor (or -1).
 * @param pipe_out Output file descriptor (or -1).
 */
void execute_command(t_cmd *cmd, int pipe_in, int pipe_out)
{
	pid_t pid;
	extern char **environ;

	// Always fork for pipeline commands
	pid = fork();
	if (pid == 0)
	{
		// Child process
		setup_child_signals();
		handle_redirections(pipe_in, pipe_out, cmd->redirections);

		if (is_builtin(cmd->args))
		{
			exec_builtin(cmd->args);
			exit(g_exit_status); // Make sure to exit after builtin
		}
		else
		{
			char *cmd_path = get_cmd_path(cmd->args[0]);
			if (!cmd_path)
			{
				ft_putstr_fd("minishell: command not found: ", STDERR_FILENO);
				ft_putstr_fd(cmd->args[0], STDERR_FILENO);
				ft_putstr_fd("\n", STDERR_FILENO);
				exit(CMD_NOT_FOUND);
			}

			// Execute the command with the current environment
			execve(cmd_path, cmd->args, environ);

			// If execve returns, it failed
			perror("minishell");
			free(cmd_path);
			exit(EXIT_FAILURE);
		}
	}
	else if (pid < 0)
	{
		perror("fork");
	}
}

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
	int status;
	int i = 0;

	if (cmd_count == 0)
		return 1;
	child_pids = malloc(sizeof(pid_t) * cmd_count);
	if (!child_pids)
		return 1;

	t_cmd *current = pipeline;
	while (current)
	{
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
			handle_redirections(prev_pipe[0],
								(current->next ? next_pipe[1] : -1),
								current->redirections);

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
		int wpid = waitpid(-1, &status, 0);
		if (wpid == -1)
		{
			if (errno == ECHILD)
				break;
			if (errno == EINTR)
				continue;
			perror("minishell: waitpid");
			break;
		}
		child_count++;

		// Update status for last command
		if (WIFEXITED(status) && child_count == cmd_count)
			last_status = WEXITSTATUS(status);
	}

	free(child_pids);
	set_exit_status(last_status);
	return last_status;
}

/*
** Sets up file descriptors for I/O redirection
** @param pipe_in Input file descriptor
** @param pipe_out Output file descriptor
** @param redirections List of redirection specifications
** @return 0 on success, -1 on error
*/
int handle_redirections(int pipe_in, int pipe_out, t_redir *redirections)
{
	// Handle pipe redirections first
	if (pipe_in != -1)
	{
		if (dup2(pipe_in, STDIN_FILENO) == -1)
			return (perror("minishell"), -1);
		close(pipe_in);
	}
	if (pipe_out != -1)
	{
		if (dup2(pipe_out, STDOUT_FILENO) == -1)
			return (perror("minishell"), -1);
		close(pipe_out);
	}

	// Handle file redirections
	t_redir *current = redirections;
	while (current)
	{
		int fd = -1;
		if (current->type == T_HEREDOC)
			fd = create_heredoc(current->filename);
		else if (current->type == T_REDIR_IN)
			fd = open(current->filename, O_RDONLY);
		else if (current->type == T_REDIR_OUT)
			fd = open(current->filename, O_WRONLY | O_CREAT | O_TRUNC, 0644);
		else if (current->type == T_APPEND)
			fd = open(current->filename, O_WRONLY | O_CREAT | O_APPEND, 0644);

		if (fd == -1)
			return (perror("minishell"), -1);

		if (current->type == T_REDIR_IN || current->type == T_HEREDOC)
		{
			if (dup2(fd, STDIN_FILENO) == -1)
				return (close(fd), perror("minishell"), -1);
		}
		else
		{
			if (dup2(fd, STDOUT_FILENO) == -1)
				return (close(fd), perror("minishell"), -1);
		}
		close(fd);
		current = current->next;
	}
	return (0);
}

/*
** Main shell loop
** @return Exit status of shell
*/
int main(void)
{
	char *input;
	t_cmd *pipeline;
	extern char **environ;
	char **env_copy;
	char **original_environ; // Save the original environ pointer

	original_environ = environ; // Save the original environ
	env_copy = ft_copy_env(environ);
	environ = env_copy; // Replace environ with the copy
	setup_parent_signals();

	while (1)
	{
		input = readline("minishell> ");
		if (!input) // Handle Ctrl+D
		{
			environ = original_environ; // Restore original environ
			free_env_copy(env_copy);	// Free the environment copy
			rl_clear_history();
			rl_reset_terminal(NULL);
			write(STDOUT_FILENO, "exit\n", 5);
			return (0);
		}

		if (ft_strlen(input) > 0)
			add_history(input);
		else
		{
			free(input);
			continue;
		}

		t_parse parser;
		init_parser(&parser, input);
		pipeline = parse_pipeline(&parser);

		if (!pipeline || !pipeline->args)
		{
			free(input);
			free_pipeline(pipeline);
			setup_parent_signals();
			continue;
		}

		// Handle "exit" directly in the parent process
		if (pipeline && !pipeline->next && is_builtin(pipeline->args))
		{
			int saved_stdin = dup(STDIN_FILENO);
			int saved_stdout = dup(STDOUT_FILENO);
			int saved_stderr = dup(STDERR_FILENO);

			// Apply redirections (no pipes: -1, -1)
			if (handle_redirections(-1, -1, pipeline->redirections) != 0)
			{
				set_exit_status(1); // Redirection error
				close(saved_stdin);
				close(saved_stdout);
				close(saved_stderr);
			}
			else
			{
				int result = exec_builtin(pipeline->args);
				set_exit_status(result);

				// Restore original descriptors
				dup2(saved_stdin, STDIN_FILENO);
				dup2(saved_stdout, STDOUT_FILENO);
				dup2(saved_stderr, STDERR_FILENO);
			}

			// Close saved descriptors
			close(saved_stdin);
			close(saved_stdout);
			close(saved_stderr);

			if (ft_strcmp(pipeline->args[0], "exit") == 0)
			{
				free(input);
				free_pipeline(pipeline);
				if (get_exit_status() == 1)
					continue;
				else
				{
					environ = original_environ; // Restore original environ
					free_env_copy(env_copy);	// Free the environment copy
					exit(get_exit_status());
				}
			}
		}
		else
		{
			execute_pipeline(pipeline);
		}

		free(input);
		free_pipeline(pipeline);
		setup_parent_signals();
	}

	environ = original_environ; // Restore original environ before exiting
	free_env_copy(env_copy);	// Free the environment copy
	rl_clear_history();			// Clear history entries
	rl_reset_terminal(NULL);
	return 0;
}
