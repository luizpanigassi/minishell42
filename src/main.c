/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: luinasci <luinasci@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/11 15:43:31 by jcologne          #+#    #+#             */
/*   Updated: 2025/03/28 18:33:30 by luinasci         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

extern char **environ;

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
	char *path;
	int result;

	// 🚨 Only handle built-ins without redirections/pipes in the parent
	if (is_builtin(cmd->args) && cmd->next == NULL && cmd->redirections == NULL)
	{
		// Execute built-in directly in parent (e.g., "exit", "cd")
		result = exec_builtin(cmd->args);
		if (result == 2) // "exit" command
		{
			printf("Exiting minishell, goodbye!\n");
			exit(0);
		}
		return;
	}

	// Fork for all other cases (external commands or built-ins with redirections/pipes)
	pid = fork();
	if (pid == 0)
	{
		setup_child_signals();

		// Handle pipe redirections
		if (pipe_in != STDIN_FILENO)
		{
			dup2(pipe_in, STDIN_FILENO);
			close(pipe_in);
		}
		if (pipe_out != STDOUT_FILENO)
		{
			dup2(pipe_out, STDOUT_FILENO);
			close(pipe_out);
		}

		// Process file redirections
		t_redir *redir = cmd->redirections;
		while (redir)
		{
			int fd;
			if (redir->type == T_REDIR_OUT)
				fd = open(redir->filename, O_WRONLY | O_CREAT | O_TRUNC, 0644);
			else if (redir->type == T_APPEND)
				fd = open(redir->filename, O_WRONLY | O_CREAT | O_APPEND, 0644);
			else if (redir->type == T_REDIR_IN)
				fd = open(redir->filename, O_RDONLY);
			else if (redir->type == T_HEREDOC)
				fd = create_heredoc(redir->filename);

			if (fd == -1)
			{
				perror("minishell: open");
				exit(EXIT_FAILURE);
			}

			if (redir->type == T_REDIR_IN || redir->type == T_HEREDOC)
				dup2(fd, STDIN_FILENO);
			else
				dup2(fd, STDOUT_FILENO);
			close(fd);
			redir = redir->next;
		}

		// Execute built-in in child (e.g., "echo" with redirections)
		if (is_builtin(cmd->args))
		{
			result = exec_builtin(cmd->args);
			exit(result);
		}

		// External command
		path = get_cmd_path(cmd->args[0]);
		if (!path)
		{
			ft_putstr_fd("minishell: ", STDERR_FILENO);
			ft_putstr_fd(cmd->args[0], STDERR_FILENO);
			ft_putstr_fd(": command not found\n", STDERR_FILENO);
			exit(CMD_NOT_FOUND);
		}
		execve(path, cmd->args, environ);
		perror("minishell");
		exit(EXIT_FAILURE);
	}
	else if (pid < 0)
	{
		perror("minishell: fork");
	}
}

int execute_pipeline(t_cmd *pipeline)
{
	int prev_pipe[2] = {-1, -1};
	int next_pipe[2];
	t_cmd *current = pipeline;
	int status;

	while (current)
	{
		// Create pipe if there's a next command
		if (current->next && pipe(next_pipe) < 0)
		{
			perror("pipe");
			return -1;
		}

		// Execute current command
		execute_command(current,
						prev_pipe[0],
						current->next ? next_pipe[1] : STDOUT_FILENO);

		// Close previous pipe ends
		if (prev_pipe[0] != -1)
			close(prev_pipe[0]);
		close(prev_pipe[1]);

		// Prepare for next iteration
		if (current->next)
		{
			prev_pipe[0] = next_pipe[0];
			prev_pipe[1] = next_pipe[1];
		}

		current = current->next;
	}

	// Wait for all children
	while (waitpid(-1, &status, 0) > 0)
		;

	return WEXITSTATUS(status);
}

int main(void)
{
	char *input;
	t_cmd *pipeline;

	setup_parent_signals();
	while (1)
	{
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
		if (is_builtin(pipeline->args) && ft_strcmp(pipeline->args[0], "exit") == 0)
		{
			int result = exec_builtin(pipeline->args);
			if (result == 2) // Exit code
			{
				free(input);
				free_pipeline(pipeline);
				exit(0); // Exit parent process
			}
		}
		else
		{
			execute_pipeline(pipeline);
		}

		free(input);
		free_pipeline(pipeline);
	}
	return 0;
}
