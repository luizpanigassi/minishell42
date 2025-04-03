/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: luinasci <luinasci@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/26 18:05:42 by luinasci          #+#    #+#             */
/*   Updated: 2025/04/03 18:01:57 by luinasci         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

extern volatile sig_atomic_t g_exit_status;

int is_builtin(char **tokens)
{
	const char *builtins[] = {"echo", "cd", "pwd", "export",
							  "unset", "env", "exit", NULL};
	int i;

	if (!tokens || !tokens[0])
		return (0);
	i = -1;
	while (builtins[++i])
	{
		if (ft_strcmp(tokens[0], builtins[i]) == 0)
			return (1);
	}
	return (0);
}

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
		return (exec_exit(args));
	return (0);
}

int exec_cd(char **args)
{
	char *oldpwd = getcwd(NULL, 0);
	char *path = NULL;
	char *newpwd;

	if (!oldpwd)
		return (perror("cd"), 1);

	if (!args[1] || ft_strcmp(args[1], "~") == 0)
		path = getenv("HOME");
	else if (ft_strcmp(args[1], "-") == 0)
		path = getenv("OLDPWD");
	else
		path = args[1];

	if (!path || chdir(path) != 0)
	{
		ft_putstr_fd("minishell: cd: ", STDERR_FILENO);
		ft_putstr_fd(args[1], STDERR_FILENO);
		ft_putstr_fd(": ", STDERR_FILENO);
		ft_putstr_fd(strerror(errno), STDERR_FILENO);
		ft_putstr_fd("\n", STDERR_FILENO);
		free(oldpwd);
		return (1);
	}

	newpwd = getcwd(NULL, 0);
	if (!newpwd)
	{
		free(oldpwd);
		return (perror("cd"), 1);
	}

	update_env_var("OLDPWD", oldpwd);
	update_env_var("PWD", newpwd);
	free(oldpwd);
	free(newpwd);
	return (0);
}

int exec_exit(char **args)
{
	int status = 0;

	if (args[1] && args[2])
	{
		ft_putstr_fd("minishell: exit error: too many arguments\n", STDERR_FILENO);
		return (1); // Return error code 1 (shell does NOT exit)
	}

	if (args[1])
	{
		if (ft_isnumber(args[1]))
			status = ft_atoi(args[1]);
		else
		{
			ft_putstr_fd("minishell: exit error: numeric argument required\n", STDERR_FILENO);
			status = 2;
			exit(status);
		}
	}
	printf("Exiting minishell, goodbye!\n");
	exit(status);
}

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

int exec_pwd(char **args)
{
	char *cwd;

	(void)args;
	cwd = getcwd(NULL, 0); // Let getcwd allocate memory
	if (cwd)
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
			// Strip surrounding quotes if present
			if (value && value[0] == '"' && value[ft_strlen_size(value) - 1] == '"')
			{
				char *tmp = ft_substr(value, 1, ft_strlen_size(value) - 2);
				free(value);
				value = tmp;
			}
		}
		else
			var_name = ft_strdup(args[i]);

		if (!is_valid_var_name(var_name))
		{
			ft_putstr_fd("minishell: export: `", STDERR_FILENO);
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

// builtin.c
char **ft_copy_env(char **original)
{
	int count = 0;
	while (original[count])
		count++;

	char **copy = malloc(sizeof(char *) * (count + 1));
	if (!copy)
		return (NULL);

	int i = 0;
	while (original[i])
	{
		copy[i] = ft_strdup(original[i]);
		i++;
	}
	copy[count] = NULL;
	return (copy);
}

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
			ft_putstr_fd("minishell: unset: `", STDERR_FILENO);
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

void update_env_var(char *var, char *value)
{
	extern char **environ;
	char *new_entry;
	char **env_ptr = environ;

	// 1. Create new entry (e.g., "VAR=value" or "VAR=")
	new_entry = (value) ? ft_strjoin3(var, "=", value) : ft_strjoin(var, "=");

	// 2. Find and replace existing entry
	while (*env_ptr)
	{
		char *eq = ft_strchr(*env_ptr, '=');
		if (eq && ft_strncmp(*env_ptr, var, eq - *env_ptr) == 0)
		{
			free(*env_ptr);
			*env_ptr = new_entry;
			free(var);
			free(value);
			return;
		}
		env_ptr++;
	}

	// 3. Add new entry
	environ = ft_array_append(environ, new_entry);
	free(var);
	free(value);
}

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
