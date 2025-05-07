/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: luinasci <luinasci@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/11 15:43:31 by jcologne          #+#    #+#             */
/*   Updated: 2025/05/07 17:13:24 by luinasci         ###   ########.fr       */
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
	t_redir *redir;
	t_redir *tmp_redir;

	while (pipeline)
	{
		current = pipeline;
		pipeline = pipeline->next;
		if (current->args)
			ft_free_array(current->args);
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
 * @brief Executes a non-builtin command.
 * @param cmd Command structure to execute.
 * @param environ Environment variables.
 */
void execute_non_builtin(t_cmd *cmd, char **environ)
{
	char *cmd_path;

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
 * @brief Executes a single command with I/O redirection.
 * @param cmd Command structure to execute.
 * @param pipe_in Input file descriptor (or -1).
 * @param pipe_out Output file descriptor (or -1).
 */
void execute_command(t_cmd *cmd, int pipe_in, int pipe_out)
{
	pid_t pid;
	extern char **environ;

	pid = fork();
	if (pid == 0)
	{
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
	else if (pid < 0)
	{
		perror("fork");
	}
}



/**
 * @brief Main shell entry point.
 * @note Manages readline loop, signal handling, and cleanup.
 */

int main(void)
{
	extern char **environ;
	char **original_environ = environ;
	char **env_copy = ft_copy_env(environ);
	char *input;
	int exit_status = 0;
	int should_exit = 0;

	// Use our environment copy
	environ = env_copy;
	setup_parent_signals();

	while (!should_exit)
	{
		input = readline("minishell> ");
		if (!input) // Handle Ctrl+D
		{
			ft_putstr_fd("Exiting minishell, goodbye!\n", STDOUT_FILENO);
			free_env_copy(env_copy);
			should_exit = 1;
			break;
		}
		if (ft_strlen(input) > 0)
			add_history(input);
		else
		{
			free(input);
			continue;
		}
		char **commands = split_with_quotes(input, ';');
		free(input);
		int syntax_error_flag = 0;
		int i = 0;
		while (commands && commands[i] && !syntax_error_flag && !should_exit)
		{
			char *trimmed_cmd = ft_strtrim(commands[i], " \t\n");
			if (!trimmed_cmd || *trimmed_cmd == '\0')
			{
				free(trimmed_cmd);
				i++;
				continue;
			}
			t_parse parser;
			init_parser(&parser, trimmed_cmd);
			t_cmd *pipeline = parse_pipeline(&parser);
			free(trimmed_cmd);
			if (parser.syntax_error)
			{
				syntax_error_flag = 1;
				set_exit_status(SYNTAX_ERROR);
				free_pipeline(pipeline);
				i++;
				continue;
			}
			if (!pipeline)
			{
				i++;
				continue;
			}
			// Handle builtins in parent
			if (!pipeline->next && is_builtin(pipeline->args))
			{
				int saved_stdin = dup(STDIN_FILENO);
				int saved_stdout = dup(STDOUT_FILENO);
				int saved_stderr = dup(STDERR_FILENO);

				if (handle_redirections(-1, -1, pipeline->redirections) == 0)
				{
					int exit_code = exec_builtin(pipeline->args);
					if (exit_code != 4242)		   // Special exit code for shell termination
						g_exit_status = exit_code; // Update global exit status
				}

				dup2(saved_stdin, STDIN_FILENO);
				dup2(saved_stdout, STDOUT_FILENO);
				dup2(saved_stderr, STDERR_FILENO);
				close(saved_stdin);
				close(saved_stdout);
				close(saved_stderr);
			}
			else
			{
				int pipeline_status = execute_pipeline(pipeline);
				if (pipeline_status == EXIT_CODE_EXIT)
				{
					should_exit = 1;
					exit_status = get_exit_status();
				}
			}

			free_pipeline(pipeline);
			i++;
		}

		if (commands)
		{
			char **ptr = commands;
			while (*ptr)
				free(*ptr++);
			free(commands);
		}

		if (syntax_error_flag)
			set_exit_status(SYNTAX_ERROR);
	}
	// Cleanup
	rl_clear_history();
	environ = original_environ;
	return (exit_status);
}
