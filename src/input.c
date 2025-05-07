/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   input.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: luinasci <luinasci@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/27 18:13:53 by luinasci          #+#    #+#             */
/*   Updated: 2025/05/07 16:55:27 by luinasci         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

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
char	*expand_variable(const char *input, size_t *i)
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
 * @brief Expands a variable or `$?` and appends it to the result string.
 * @param input The input string to process.
 * @param i Pointer to the current index in the input string.
 * @param result Pointer to the result string being built.
 * @return Updated result string with the expanded variable appended.
 */
char	*expand_and_append_variable(const char *input, size_t *i, char *result)
{
	char	*var_value;

	(*i)++;
	if (input[*i])
	{
		var_value = expand_variable(input, i);
		result = ft_strjoin_free(result, var_value);
		free(var_value);
	}
	return (result);
}

/**
 * @brief Expands environment variables and `$?` in a string.
 * @param input The input string to process.
 * @return char* New string with variables expanded. Must be freed by the caller.
 */
char	*expand_variables(const char *input)
{
	char	*result;
	size_t	i;

	i = 0;
	result = ft_strdup("");
	while (input[i])
	{
		if (input[i] == '$' && (input[i + 1] == '?'
				|| ft_isalpha(input[i + 1]) || input[i + 1] == '_'))
			result = expand_and_append_variable(input, &i, result);
		else
			result = ft_strjoin_char(result, input[i++]);
		if (!result)
			return (NULL);
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

	if (!cmd->args[0])
	{
		ft_putstr_fd("minishell: : command not found\n", STDERR_FILENO);
		exit(CMD_NOT_FOUND);
	}
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
