/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils_7.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: luinasci <luinasci@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/07 16:47:09 by luinasci          #+#    #+#             */
/*   Updated: 2025/05/13 15:24:41 by luinasci         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/**
 * @brief Splits string while respecting quoted sections.
 * @param str Input string to split.
 * @param delim Delimiter character.
 * @return Array of split tokens.
 * @note Preserves quoted content including delimiters.
 */
char	**split_with_quotes(const char *str, char delim)
{
	const char	*start;
	char		**result;
	int			count;
	int			in_quote;
	char		quote_char;

	start = str;
	result = NULL;
	count = 0;
	in_quote = 0;
	quote_char = '\0';
	while (*str)
	{
		str = skip_escaped(str, delim);
		if (!*str)
			break ;
		process_quote_state(str, &in_quote, &quote_char);
		if (*str == delim && !in_quote)
			handle_delimiter(&str, &start, &result, &count);
		str++;
	}
	finalize_split(start, str, &result, &count);
	if (result)
		result[count] = NULL;
	return (result);
}

/**
 * @brief Global error handler with exit.
 * @param message Error description to display.
 * @note Prints to stderr and exits with failure status.
 */
void	handle_error(char *message)
{
	perror(message);
	exit(EXIT_FAILURE);
}

/**
 * @brief Reads user input and splits it into commands.
 * @param env_copy Copy of the environment variables.
 * @param should_exit Pointer to the exit flag.
 * @return Array of command strings or NULL if the shell should exit.
 */
char	**read_and_split_input(char **env_copy, int *should_exit)
{
	char	*input;
	char	**commands;

	input = readline("minishell> ");
	if (!input)
	{
		ft_putstr_fd("Exiting minishell, goodbye!\n", STDOUT_FILENO);
		free_env_copy(env_copy);
		*should_exit = 1;
		return (NULL);
	}
	if (ft_strlen(input) > 0)
		add_history(input);
	else
	{
		free(input);
		return (NULL);
	}
	commands = split_with_quotes(input, ';');
	free(input);
	return (commands);
}

/**
 * @brief Cleans up resources and handles errors during pipeline execution.
 * @param child_pids Array of child process IDs.
 * @param pipeline Linked list of commands to execute.
 * @param exit_code Exit code to return after cleanup.
 * @return The provided exit code.
 */
int	cleanup_on_failure(pid_t **child_pids_ptr, int exit_code)
{
	if (*child_pids_ptr)
	{
		free(*child_pids_ptr);
		*child_pids_ptr = NULL;
	}
	return (exit_code);
}

/**
 * @brief Frees an array of command strings.
 * @param commands Array of command strings to free.
 */
void	free_commands(char **commands)
{
	char	**ptr;

	ptr = commands;
	if (!commands)
		return ;
	while (*ptr)
		free(*ptr++);
	free(commands);
}
