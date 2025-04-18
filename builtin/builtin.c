/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: luinasci <luinasci@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/26 18:05:42 by luinasci          #+#    #+#             */
/*   Updated: 2025/04/18 16:57:04 by luinasci         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

extern volatile	sig_atomic_t g_exit_status;

/*
** Checks if a command is a builtin
** @param tokens Array of command tokens
** @return 1 if builtin, 0 otherwise
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

/*
** Executes the appropriate builtin function based on command
** @param args Command arguments
** @return Exit status of the builtin command
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

/*
** Prints the current environment variables
** @param args Ignored
** @return Always returns 0
*/
int	exec_env(char **args)
{
	extern char	**environ;
	int			i;

	(void)args;
	i = -1;
	while (environ[++i])
		printf("%s\n", environ[i]);
	return (0);
}

/**
 * @brief Executes the 'export' builtin to set environment variables.
 * @param args Array of arguments (e.g., ["export", "VAR=value"]).
 * @return 0 on success, 1 if any invalid variable name is encountered.
 */
int exec_export(char **args)
{
	extern char	**environ;
	char		*eq_pos;
	char		*var_name;
	char		*value;
	int			ret;
	int			i;

	if (!args[1])
	{
		print_export_declarations();
		return (0);
	}
	ret = 0;
	i = 1;
	while (args[i])
	{
		eq_pos = ft_strchr(args[i], '=');
		var_name = NULL;
		value = NULL;
		if (eq_pos)
		{
			var_name = ft_substr(args[i], 0, eq_pos - args[i]);
			value = ft_strdup(eq_pos + 1);
		}
		else
			var_name = ft_strdup(args[i]);

		if (!is_valid_var_name(var_name))
		{
			ft_putstr_fd("minishell: export: '", STDERR_FILENO);
			ft_putstr_fd(args[i], STDERR_FILENO);
			ft_putstr_fd("': not a valid identifier\n", STDERR_FILENO);
			ret = 1;
			free(var_name);
			free(value);
			i++;
			continue ;
		}
		if (eq_pos)
			update_env_var(var_name, value);
		else
			ensure_var_exported(var_name);
		i++;
	}
	return (ret);
}

/*
** Creates a copy of the environment variables array
** @param original Original environment array
** @return New allocated copy of the environment
*/
char	**ft_copy_env(char **original)
{
	char	**copy;
	int		i;

	if (!original)
		return (NULL);
	i = 0;
	while (original[i])
		i++;
	copy = malloc(sizeof(char *) * (i + 1));
	if (!copy)
		return (NULL);
	i = 0;
	while (original[i])
	{
		copy[i] = ft_strdup(original[i]);
		if (!copy[i])
		{
			ft_free_array(copy);
			return (NULL);
		}
		i++;
	}
	copy[i] = NULL;
	return (copy);
}

/**
 * @brief Executes the 'unset' builtin to remove environment variables.
 * @param args Array of arguments (e.g., ["unset", "VAR"]).
 * @return 0 on success, 1 if any invalid variable name is encountered.
 */
int exec_unset(char **args)
{
	extern char **environ;
	int ret;
	int i;

	if (!args[1])
		return (0);

	ret = 0;
	i = 1;
	while (args[i])
	{
		if (!is_valid_var_name(args[i]))
		{
			ft_putstr_fd("minishell: unset: '", STDERR_FILENO);
			ft_putstr_fd(args[i], STDERR_FILENO);
			ft_putstr_fd("': not a valid identifier\n", STDERR_FILENO);
			ret = 1;
			i++;
			continue;
		}

		char **env_ptr = environ;
		while (*env_ptr)
		{
			char *eq = ft_strchr(*env_ptr, '=');
			size_t var_len = eq ? eq - *env_ptr : ft_strlen(*env_ptr);

			if (ft_strncmp(*env_ptr, args[i], var_len) == 0 && var_len == ft_strlen_size(args[i]))
			{
				free(*env_ptr);
				char **ptr = env_ptr;
				while (*ptr)
				{
					*ptr = *(ptr + 1);
					ptr++;
				}
				break;
			}
			env_ptr++;
		}
		i++;
	}
	return (ret);
}

/* Environment variable operations */
/**
 * @brief Updates or adds an environment variable.
 * @param var Variable name (ownership transferred).
 * @param value Variable value (ownership transferred).
 */
void update_env_var(char *var, char *value)
{
	extern char	**environ;
	char		*new_entry;
	char		*eq;
	char		**env_ptr;

	new_entry = NULL;
	if (value)
		new_entry = ft_strjoin3(var, "=", value);
	else
		new_entry = ft_strjoin(var, "=");
	env_ptr = environ;
	while (*env_ptr)
	{
		eq = ft_strchr(*env_ptr, '=');
		if (eq && ft_strncmp(*env_ptr, var, eq - *env_ptr) == 0)
		{
			free(*env_ptr);
			*env_ptr = new_entry;
			free(value);
			return ;
		}
		env_ptr++;
	}
	environ = ft_array_append(environ, new_entry);
	free(value);
}

/*
** Ensures a variable exists in the environment (without value if not present)
** @param var_name Variable name to check/export
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
