/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: luinasci <luinasci@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/11 15:43:31 by jcologne          #+#    #+#             */
/*   Updated: 2025/03/27 18:56:38 by luinasci         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

extern char **environ; // Add this for environment access

void free_args(char **args)
{
	int i;

	if (!args)
		return;
	i = 0;
	while (args[i])
	{
		free(args[i]);
		i++;
	}
	free(args);
}

int main(void)
{
	char *input;
	char **tokens;
	pid_t pid;
	char *path;
	t_parse parser;

	setup_parent_signals();
	while (1)
	{
		input = readline("$minishell> ");
		if (!input) // Handle Ctrl+D
		{
			ft_putstr_fd("exit\n", STDOUT_FILENO);
			break;
		}
		add_history(input);

		// Parse input into tokens
		init_parser(&parser, input);
		tokens = parse_args(&parser);
		//debug print

		if (!tokens || !tokens[0]) // Handle empty input/parsing errors
		{
			free(input);
			free_args(tokens);
			continue;
		}

		if (is_builtin(tokens)) // Use tokens instead of args
		{
			// Execute builtin and cleanup
			free_args(tokens);
			free(input);
			continue;
		}

		pid = fork();
		if (pid == -1)
		{
			perror("fork");
			free_args(tokens);
			free(input);
			continue;
		}
		else if (pid == 0) // Child process
		{
			setup_child_signals();
			path = get_cmd_path(tokens[0]); // Use tokens[0]
			if (!path)
			{
				ft_putstr_fd("minishell: ", STDERR_FILENO);
				ft_putstr_fd(tokens[0], STDERR_FILENO);
				ft_putstr_fd(": command not found\n", STDERR_FILENO);
				free_args(tokens);
				free(input);
				exit(CMD_NOT_FOUND);
			}

			if (execve(path, tokens, environ) == -1) // Use environ
			{
				perror("minishell");
				free(path);
				free_args(tokens);
				free(input);
				exit(EXIT_FAILURE);
			}
		}
		else // Parent process
		{
			waitpid(pid, NULL, 0);
			free_args(tokens);
			free(input);
		}
	}
	return (0);
}
