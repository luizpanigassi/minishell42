/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: luinasci <luinasci@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/26 18:05:42 by luinasci          #+#                #+#             */
/*   Updated: 2025/04/14 17:34:17 by luinasci         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

extern volatile sig_atomic_t g_exit_status;

/*
** Checks if a command is a builtin
** @param tokens Array of command tokens
** @return 1 if builtin, 0 otherwise
*/
int is_builtin(char **tokens)
{
	const char *builtins[] = {"echo", "cd", "pwd", "export",
							  "unset", "env", "exit", NULL};
	int i;

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
int exec_builtin(char **args)
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
** Changes the current directory
** @param args Command arguments (args[1] is the target directory)
** @return 0 on success, 1 on failure
*/
int exec_cd(char **args)
{
	char *oldpwd = getcwd(NULL, 0);
	char *path = NULL;
	char *newpwd;

	if (!oldpwd)
		return (perror("cd"), 1);

	// Check for too many arguments
	if (args[1] && args[2])
	{
		ft_putstr_fd("minishell: cd: too many arguments\n", STDERR_FILENO);
		free(oldpwd);
		set_exit_status(1);
		return (1);
	}
	// Determine path (existing logic)
	if (!args[1] || ft_strcmp(args[1], "~") == 0)
		path = getenv("HOME");
	else if (ft_strcmp(args[1], "-") == 0)
		path = getenv("OLDPWD");
	else
		path = args[1];

	// Handle errors (existing error checking)
	if (chdir(path) != 0)
	{
		// ... error message logic ...
		perror("cd");
		set_exit_status(1);
		free(oldpwd);
		return (1);
	}

	newpwd = getcwd(NULL, 0);
	if (!newpwd)
	{
		free(oldpwd);
		return (perror("cd"), 1);
	}
	// Update environment - DON'T FREE THESE AFTER
	update_env_var("OLDPWD", oldpwd);
	update_env_var("PWD", newpwd);
	return 0;
}

/*
** Exits the shell with optional status code
** @param args Command arguments (args[1] is optional exit status)
** @return Does not return on success, returns 1 if too many arguments
*/
int exec_exit(char **args)
{
	int status;

	status = 0;
	if (args[1] && args[2])
	{
		ft_putstr_fd("minishell: exit error: too many arguments\n", STDERR_FILENO);
		return (1); // Return error code 1 (shell does NOT exit)
	}

	if (args[1])
	{
		if (ft_isnumber(args[1]))
		{
			status = ft_atoi(args[1]);
		}
		else
		{
			ft_putstr_fd("minishell: exit error: numeric argument required\n", STDERR_FILENO);
			status = 2;
			exit(status);
		}
	}
	printf("Exiting minishell, goodbye!\n");
	rl_clear_history();
	exit(status);
}

/*
** Prints arguments to stdout with optional newline suppression
** @param args Command arguments (-n flag suppresses newline)
** @return Always returns 0
*/
int exec_echo(char **args) // use -n
{
	int i;
	int nl_flag;

	i = 1;
	nl_flag = 0;
	if (args[1] && ft_strcmp(args[1], "-n") == 0)
	{
		nl_flag = 1;
		i++;
	}
	while (args[i])
	{
		ft_putstr_fd(args[i], STDOUT_FILENO);
		if (args[++i])
			ft_putchar_fd(' ', STDOUT_FILENO);
	}
	if (!nl_flag)
		ft_putchar_fd('\n', STDOUT_FILENO);
	return (0);
}

/*
** Prints the current environment variables
** @param args Ignored
** @return Always returns 0
*/
int exec_env(char **args)
{
	extern char **environ;
	int i;

	(void)args;
	i = -1;
	while (environ[++i])
		printf("%s\n", environ[i]);
	return (0);
}

/*
** Prints the current working directory
** @param args Ignored
** @return 0 on success, 1 on failure
*/
int exec_pwd(char **args)
{
	char *cwd;

	(void)args;
	cwd = getcwd(NULL, 0); // Let getcwd allocate memory
	if (args[1])
	{
		ft_putstr_fd("minishell: pwd: too many arguments\n", STDERR_FILENO);
		free(cwd);
		return (1);
	}
	if (cwd && args[1] == NULL)
	{
		printf("%s\n", cwd);
		free(cwd);
		return (0);
	}
	else
	{
		perror("minishell: pwd");
		return (1);
	}
}

/**
 * @brief Executes the 'export' builtin to set environment variables.
 * @param args Array of arguments (e.g., ["export", "VAR=value"]).
 * @return 0 on success, 1 if any invalid variable name is encountered.
 */
int exec_export(char **args)
{
	extern char **environ;

	if (!args[1])
	{
		print_export_declarations();
		return (0);
	}

	int ret = 0;
	int i = 1;
	while (args[i])
	{
		char *eq_pos = ft_strchr(args[i], '=');
		char *var_name = NULL;
		char *value = NULL;

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
			free(var_name); // Free only on error
			free(value);	// Free only on error
			i++;
			continue;
		}

		if (eq_pos)
			update_env_var(var_name, value); // Ownership transferred
		else
			ensure_var_exported(var_name); // Ownership transferred
		i++;
	}
	return (ret);
}

/*
** Creates a copy of the environment variables array
** @param original Original environment array
** @return New allocated copy of the environment
*/
char **ft_copy_env(char **original)
{
	char **copy;
	int i;

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
		if (!copy[i]) // Handle allocation failure
		{
			ft_free_array(copy); // Free previously allocated memory
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

	if (!args[1])
		return (0);

	int ret = 0;
	int i = 1;
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
	extern char **environ;
	char *new_entry = NULL;

	// Create new environment entry
	if (value)
		new_entry = ft_strjoin3(var, "=", value);
	else
		new_entry = ft_strjoin(var, "=");

	// Find and replace existing entry
	char **env_ptr = environ;
	while (*env_ptr)
	{
		char *eq = ft_strchr(*env_ptr, '=');
		if (eq && ft_strncmp(*env_ptr, var, eq - *env_ptr) == 0)
		{
			free(*env_ptr);
			*env_ptr = new_entry;
			free(value); // Only free the value parameter
			return;
		}
		env_ptr++;
	}

	// Add new entry
	environ = ft_array_append(environ, new_entry);
	free(value); // Only free the value parameter
}

/*
** Ensures a variable exists in the environment (without value if not present)
** @param var_name Variable name to check/export
*/
void ensure_var_exported(char *var_name)
{
	extern char **environ;
	char **env_ptr = environ;
	int exists = 0;

	while (*env_ptr && !exists)
	{
		char *eq = ft_strchr(*env_ptr, '=');
		if (eq && ft_strncmp(*env_ptr, var_name, eq - *env_ptr) == 0)
			exists = 1;
		env_ptr++;
	}
	if (!exists)
		update_env_var(var_name, NULL);
	else
		free(var_name); // Free here since update_env_var no longer owns it
}
