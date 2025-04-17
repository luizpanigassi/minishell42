/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   input.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jcologne <jcologne@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/27 18:13:53 by luinasci          #+#    #+#             */
/*   Updated: 2025/04/17 14:54:02 by jcologne         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/*
** Prints error message and exits
** @param message Error message to display
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
char	*expand_variables(const char *input)
{
	char	*exit_status;
	char	*result;
	size_t	i;
	size_t	start;
	char	*var_name;
	char	*var_value;

	i = 0;
	result = ft_strdup("");
	while (input[i])
	{
		if (input[i] == '$' && (input[i + 1] == '?' || ft_isalpha(input[i + 1]) || input[i + 1] == '_'))
		{
			i++;
			if (input[i] == '?')
			{
				exit_status = ft_itoa(get_exit_status());
				result = ft_strjoin_free(result, exit_status);
				free(exit_status);
				i++;
			}
			else
			{
				start = i;
				while (input[i] && (ft_isalnum(input[i]) || input[i] == '_'))
					i++;
				var_name = ft_substr(input, start, i - start);
				var_value = getenv(var_name);
				if (var_value)
					result = ft_strjoin_free(result, var_value);
				free(var_name);
			}
		}
		else
		{
			result = ft_strjoin_char(result, input[i++]);
		}
	}
	return (result);
}

/**
 * @brief Executes an external command in a child process.
 * @param cmd Command structure with arguments and redirections.
 * @note Terminates the process on completion or error.
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
