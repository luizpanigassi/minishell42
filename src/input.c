/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   input.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: luinasci <luinasci@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/27 18:13:53 by luinasci          #+#    #+#             */
/*   Updated: 2025/04/14 17:33:56 by luinasci         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/*
** Prints error message and exits
** @param message Error message to display
*/
void handle_error(char *message)
{
	perror(message);
	exit(EXIT_FAILURE);
}

/**
 * @brief Locates the full path of a command using PATH.
 * @param cmd Command name (e.g., "ls").
 * @return Allocated full path (e.g., "/bin/ls") or NULL if not found.
 * Caller must free the returned string.
 */
char *get_cmd_path(char *cmd)
{
	char **paths;
	char *full_path;
	char *dir_slash;
	int i;

	// Handle absolute paths and commands with path specified
	if (ft_strchr(cmd, '/') != NULL)
	{
		if (access(cmd, F_OK) == -1)
			return NULL; // Not found
		if (access(cmd, X_OK) == -1)
		{
			g_exit_status = PERM_DENIED; // Set global error
			return NULL;
		}
		return ft_strdup(cmd);
	}
	// Get PATH environment variable
	char *path_env = getenv("PATH");
	if (!path_env)
		return (NULL);
	// Split PATH into individual directories
	paths = ft_split(path_env, ':');
	if (!paths)
		return (NULL);

	i = 0;
	while (paths[i])
	{
		// Create directory + slash
		dir_slash = ft_strjoin(paths[i], "/");
		if (!dir_slash)
		{
			ft_free_array(paths);
			return (NULL);
		}
		// Create full path
		full_path = ft_strjoin(dir_slash, cmd);
		free(dir_slash);
		if (!full_path)
		{
			ft_free_array(paths);
			return (NULL);
		}
		// Check if executable exists
		if (access(full_path, X_OK) == 0)
		{
			ft_free_array(paths);
			return (full_path);
		}
		free(full_path);
		i++;
	}
	ft_free_array(paths);
	return (NULL);
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
char *expand_variables(const char *input)
{
	char *result = ft_strdup("");
	size_t i = 0;

	while (input[i])
	{
		if (input[i] == '$' && (input[i + 1] == '?' || ft_isalpha(input[i + 1]) || input[i + 1] == '_'))
		{
			i++; // Skip $
			if (input[i] == '?')
			{
				char *exit_status = ft_itoa(get_exit_status());
				result = ft_strjoin_free(result, exit_status);
				free(exit_status);
				i++;
			}
			else
			{
				size_t start = i;
				while (input[i] && (ft_isalnum(input[i]) || input[i] == '_'))
					i++;
				char *var_name = ft_substr(input, start, i - start);
				char *var_value = getenv(var_name);
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
	return result;
}

/**
 * @brief Executes an external command in a child process.
 * @param cmd Command structure with arguments and redirections.
 * @note Terminates the process on completion or error.
 */
void exec_external_command(t_cmd *cmd)
{
	extern char **environ;

	char *path = get_cmd_path(cmd->args[0]);
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
int handle_redirection(t_redir *current, int fd)
{
	if (current->type == T_REDIR_IN || current->type == T_HEREDOC)
	{
		if (dup2(fd, STDIN_FILENO) == -1)
		{
			close(fd);
			perror("minishell");
			return -1;
		}
	}
	else
	{
		if (dup2(fd, STDOUT_FILENO) == -1)
		{
			close(fd);
			perror("minishell");
			return -1;
		}
	}
	return 0;
}
