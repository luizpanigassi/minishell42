/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   input.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: luinasci <luinasci@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/27 18:13:53 by luinasci          #+#    #+#             */
/*   Updated: 2025/04/28 19:42:34 by luinasci         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

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
 * @brief Expands environment variables and `$?` in a string.
 *
 * @param input The input string to process.
 * @return char* New string with variables expanded. Must be freed by the caller.
 *
 * @note Expansion rules:
 * - `$VAR` → Replaced with environment variable value.
 * - `$?` → Replaced with the last exit status.
 * - Single-quoted strings suppress expansion.
 * - Double-quoted strings allow `$VAR` and `$?` expansion.
 */
static char	*expand_variable(const char *input, size_t *i)
{
	char	*value;
	size_t	start;
	char	*var_name;
	char	*var_value;

	if (input[*i] == '?')
	{
		value = ft_itoa(get_exit_status());
		(*i)++;
	}
	else
	{
		start = *i;
		while (input[*i] && (ft_isalnum(input[*i]) || input[*i] == '_'))
			(*i)++;
		var_name = ft_substr(input, start, *i - start);
		var_value = getenv(var_name);
		if (var_value)
			value = ft_strdup(var_value);
		else
			value = ft_strdup("");
		free(var_name);
	}
	return (value);
}

/**
 * @brief Executes external programs via fork/exec.
 * @param cmd Command structure with arguments.
 * @note Uses PATH resolution and reports command errors.
 */

char	*expand_variables(const char *input)
{
	char	*result;
	size_t	i;
	char	*var_value;

	i = 0;
	result = ft_strdup("");
	while (input[i])
	{
		if (input[i] == '$' && (input[i + 1] == '?' || ft_isalpha(input[i + 1])
				|| input[i + 1] == '_'))
		{
			i++;
			if (input[i])
			{
				var_value = expand_variable(input, &i);
				result = ft_strjoin_free(result, var_value);
				free(var_value);
			}
		}
		else
			result = ft_strjoin_char(result, input[i++]);
	}
	return (result);
}

/**
 * @brief Executes external programs via fork/exec.
 * @param cmd Command structure with arguments.
 * @note Uses PATH resolution and reports command errors.
 */
void	exec_external_command(t_cmd *cmd)
{
	extern char	**environ;
	char		*path;

	path = get_cmd_path(cmd->args[0]);
	if (!path)
	{
		if (access(cmd->args[0], F_OK) == 0)
			exit(PERM_DENIED);
		ft_putstr_fd("minishell: ", STDERR_FILENO);
		ft_putstr_fd(cmd->args[0], STDERR_FILENO);
		ft_putstr_fd(": command not found\n", STDERR_FILENO);
		exit(CMD_NOT_FOUND);
	}
	execve(path, cmd->args, environ);
	perror("minishell");
	free(path);
	exit(EXIT_FAILURE);
}

/**
 * @brief Handles redirection for commands.
 * @param current Current redirection node.
 * @param fd File descriptor for redirection.
 * @return int 0 on success, -1 on error.
 */
int	handle_redirection(t_redir *current, int fd)
{
	if (current->type == T_REDIR_IN || current->type == T_HEREDOC)
	{
		if (dup2(fd, STDIN_FILENO) == -1)
		{
			close(fd);
			perror("minishell");
			return (-1);
		}
	}
	else
	{
		if (dup2(fd, STDOUT_FILENO) == -1)
		{
			close(fd);
			perror("minishell");
			return (-1);
		}
	}
	return (0);
}
