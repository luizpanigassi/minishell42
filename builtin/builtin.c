/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: luinasci <luinasci@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/26 18:05:42 by luinasci          #+#    #+#             */
/*   Updated: 2025/05/06 15:58:41 by luinasci         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

extern volatile sig_atomic_t	g_exit_status;

/**
 * @brief Checks if a command is a builtin command.
 * @param tokens Array of command tokens to check.
 * @return 1 if the command is a builtin, 0 otherwise.
 * @note Checks against predefined list of builtin commands.
 */
int	is_builtin(char **tokens)
{
	const char	*builtins[] = {"echo", "cd", "pwd", "export",
		"unset", "env", "exit", NULL};
	int			i;

	if (!tokens || !tokens[0])
		return (0);
	i = 0;
	while (builtins[i])
	{
		if (ft_strcmp(tokens[0], builtins[i]) == 0)
			return (1);
		i++;
	}
	return (0);
}

/**
 * @brief Executes the appropriate builtin function.
 * @param args Command arguments including builtin name.
 * @return Exit status of the executed builtin.
 * @note Dispatches to specific builtin implementations based on first argument.
 */
int	exec_builtin(char **args)
{
	if (ft_strcmp(args[0], "echo") == 0)
		return (exec_echo(args));
	if (ft_strcmp(args[0], "cd") == 0)
		return (exec_cd(args));
	if (ft_strcmp(args[0], "pwd") == 0)
		return (exec_pwd(args));
	if (ft_strcmp(args[0], "export") == 0)
		return (exec_export(args));
	if (ft_strcmp(args[0], "unset") == 0)
		return (exec_unset(args));
	if (ft_strcmp(args[0], "env") == 0)
		return (exec_env(args));
	if (ft_strcmp(args[0], "exit") == 0)
	{
		exec_exit(args);
		return (EXIT_CODE_EXIT);
	}
	return (0);
}

/**
 * @brief Creates a deep copy of the environment variables array.
 * @param original Original environment array to copy.
 * @return New allocated copy of environment variables.
 * @note Caller must free the returned array with free_env_copy().
 */
char	**ft_copy_env(char **original)
{
	char	**copy;
	int		i;
	int		count;

	i = 0;
	if (!original)
		return (NULL);
	count = 0;
	while (original[count])
		count++;
	copy = malloc(sizeof(char *) * (count + 1));
	if (!copy)
		return (NULL);
	while (i < count)
	{
		copy[i] = ft_strdup(original[i]);
		if (!copy[i])
		{
			ft_free_array(copy);
			return (NULL);
		}
		i++;
	}
	copy[count] = NULL;
	return (copy);
}

/* Environment variable operations */
/**
 * @brief Updates or adds an environment variable.
 * @param var Variable name (ownership transferred).
 * @param value Variable value (ownership transferred).
 */
void	update_env_var(char *var, char *value)
{
	extern char	**environ;
	char		*new_entry;
	char		*eq;
	char		**env_ptr;

	if (value != NULL)
		new_entry = ft_strjoin3(var, "=", value);
	else
		new_entry = ft_strjoin(var, "=");
	env_ptr = environ;
	while (*env_ptr)
	{
		eq = ft_strchr(*env_ptr, '=');
		if (eq && ft_strncmp(*env_ptr, var, eq - *env_ptr) == 0)
		{
			*env_ptr = new_entry;
			return ;
		}
		env_ptr++;
	}
	environ = ft_array_append(environ, new_entry);
	free(var);
	if (value)
		free(value);
}

/**
 * Ensures a variable exists in the environment (without value if not present)
 * @param var_name Variable name to check/export
*/
void	ensure_var_exported(char *var_name)
{
	extern char	**environ;
	char		**env_ptr;
	int			exists;
	char		*eq;

	exists = 0;
	env_ptr = environ;
	while (*env_ptr && !exists)
	{
		eq = ft_strchr(*env_ptr, '=');
		if (eq && ft_strncmp(*env_ptr, var_name, eq - *env_ptr) == 0)
			exists = 1;
		env_ptr++;
	}
	if (!exists)
		update_env_var(var_name, NULL);
	else
		free(var_name);
}
