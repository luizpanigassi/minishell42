/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jcologne <jcologne@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/11 15:43:31 by jcologne          #+#    #+#             */
/*   Updated: 2025/03/17 22:06:13 by jcologne         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

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

int	main(void)
{
	char	*input;
	char	**args;
	int 	pid;
	char 	*path;

	while (1)
	{
		input = readline("$_mini_shell_> ");
		if (!input)
		{
			printf("exit\n");
			break;
		}
		add_history(input);
		args = ft_split(input, 32);
		if (!args)
		{
			free(input);
			continue;
		}
		
		if (args[0] == NULL)
		{
			free_args(args);
			free(input);
			continue;
		}
		
		if (is_builtin(args))
		{
			// A função builtin deve lidar com a limpeza dos argumentos
			free(input);
			continue;
		}
		else
		{
			pid = fork();
			if (pid == -1)
			{
				perror("fork");
				free_args(args);
				free(input);
				continue;
			}
			else if (pid == 0)
			{
				// Processo filho
				path = get_cmd_path(args[0]);
				if (!path)
				{
					printf("Command not found: %s\n", args[0]);
					free_args(args);
					free(input);
					exit(EXIT_FAILURE);
				}
				// Executa o comando
				if (execve(path, args, NULL) == -1)
				{
					perror("Error");
					free(path);
					free_args(args);
					free(input);
					exit(EXIT_FAILURE);
				}
			}
			else
			{
				// Processo pai
				waitpid(pid, NULL, 0);
				free_args(args);
				free(input);
			}
		}
	}
	return (0);
}
