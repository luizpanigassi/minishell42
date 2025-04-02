/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: luinasci <luinasci@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/11 15:43:31 by jcologne          #+#    #+#             */
/*   Updated: 2025/04/02 18:15:09 by luinasci         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

volatile sig_atomic_t g_exit_status = 0;

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

int execute_pipeline(t_cmd *pipeline)
{
	int prev_pipe[2] = {-1, -1};
	int next_pipe[2];
	t_cmd *current = pipeline;
	pid_t last_pid = -1;
	int status;
	extern char **environ;

	while (current)
	{
		if (current->next && pipe(next_pipe) < 0)
		{
			perror("minishell: pipe");
			return (-1);
		}

		pid_t pid = fork();
		if (pid == 0) // Child process
		{
			setup_child_signals();
			handle_redirections(
				prev_pipe[0],					   // Input from previous command
				current->next ? next_pipe[1] : -1, // Output to next command
				current->redirections			   // File redirections
			);

			// Close pipe ends in child
			if (prev_pipe[0] != -1)
				close(prev_pipe[0]);
			if (current->next)
			{
				close(next_pipe[0]);
				close(next_pipe[1]);
			}

			if (is_builtin(current->args))
			{
				int result = exec_builtin(current->args);
				exit(result);
			}
			else
			{
				char *path = get_cmd_path(current->args[0]);
				if (!path)
				{
					ft_putstr_fd("minishell: ", STDERR_FILENO);
					ft_putstr_fd(current->args[0], STDERR_FILENO);
					ft_putstr_fd(": command not found\n", STDERR_FILENO);
					exit(CMD_NOT_FOUND);
				}
				execve(path, current->args, environ);
				perror("minishell");
				exit(EXIT_FAILURE);
			}
		}

		// Parent continues
		if (prev_pipe[0] != -1)
			close(prev_pipe[0]);
		close(prev_pipe[1]);

		// Prepare for next iteration
		if (current->next)
		{
			prev_pipe[0] = next_pipe[0];
			prev_pipe[1] = next_pipe[1];
		}

		if (!current->next)
			last_pid = pid;
		current = current->next;
	}

	// Wait for last command
	if (last_pid != -1)
	{
		waitpid(last_pid, &status, 0);
		g_exit_status = WEXITSTATUS(status);
	}

	// Cleanup remaining processes
	while (waitpid(-1, NULL, WNOHANG) > 0)
		;

	return (g_exit_status);
}

int handle_redirections(int pipe_in, int pipe_out, t_redir *redirections)
{
	// Handle pipe redirections
	if (pipe_in != STDIN_FILENO && pipe_in != -1)
	{
		if (dup2(pipe_in, STDIN_FILENO) == -1)
			return (perror("minishell"), -1);
		close(pipe_in);
	}
	if (pipe_out != STDOUT_FILENO && pipe_out != -1)
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

int main(void)
{
	char *input;
	t_cmd *pipeline;

	setup_parent_signals();
	while (1)
	{
		setup_parent_signals();
		input = readline("minishell> ");
		if (!input)
		{
			ft_putstr_fd("exit\n", STDOUT_FILENO);
			break;
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
			continue;
		}

		// 🚨 Handle "exit" directly in the parent process
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
				exit(get_exit_status());
			}
		}
		else
		{
			execute_pipeline(pipeline);
		}

		free(input);
		free_pipeline(pipeline);
	}
	rl_clear_history(); // Clear history entries
	rl_reset_terminal(NULL);
	return 0;
}
