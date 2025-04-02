/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: luinasci <luinasci@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/26 18:05:42 by luinasci          #+#    #+#             */
/*   Updated: 2025/04/02 16:24:58 by luinasci         ###   ########.fr       */
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

	if (args[1])
	{
		if (ft_isnumber(args[1]))
			status = ft_atoi(args[1]);
		else
		{
			ft_putstr_fd("minishell: exit: numeric argument required\n", STDERR_FILENO);
			status = 2;
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
	int i;
	int ret;
	char *eq_pos;

	if (!args[1])
		return (exec_env(args)); // Show all variables when no arguments

	ret = 0;
	i = 0;
	while (args[++i])
	{
		eq_pos = ft_strchr(args[i], '=');
		if (!eq_pos)
		{
			ft_putstr_fd("minishell: export: `", STDERR_FILENO);
			ft_putstr_fd(args[i], STDERR_FILENO);
			ft_putstr_fd("': not a valid identifier\n", STDERR_FILENO);
			ret = 1;
			continue;
		}
		*eq_pos = '\0';
		update_env_var(args[i], eq_pos + 1);
		*eq_pos = '=';
	}
	g_exit_status = ret;
	return (ret);
}

char **ft_copy_env(char **env)
{
	char **new_env;
	int count;
	int i;

	count = 0;
	while (env[count])
		count++;
	new_env = malloc(sizeof(char *) * (count + 1));
	if (!new_env)
		return (NULL);
	i = -1;
	while (++i < count)
		new_env[i] = ft_strdup(env[i]);
	new_env[count] = NULL;
	return (new_env);
}

int exec_unset(char **args)
{
	extern char **environ;
	int i = 1;

	if (!args[1])
		return 0;

	while (args[i])
	{
		int j = 0;
		while (environ[j])
		{
			if (ft_strncmp(environ[j], args[i], ft_strlen(args[i])) == 0 &&
				environ[j][ft_strlen(args[i])] == '=')
			{
				free(environ[j]);
				// Shift remaining elements
				while (environ[j])
				{
					environ[j] = environ[j + 1];
					j++;
				}
				break;
			}
			j++;
		}
		i++;
	}
	return 0;
}

// builtin.c
void update_env_var(const char *var, const char *value)
{
	char *tmp;
	char *entry;
	int i;
	extern char **environ;

	tmp = ft_strjoin(var, "=");
	entry = ft_strjoin(tmp, value);
	free(tmp);

	i = -1;
	while (environ[++i])
	{
		if (ft_strncmp(environ[i], var, ft_strlen(var)) == 0 && environ[i][ft_strlen(var)] == '=')
		{
			free(environ[i]);
			environ[i] = entry;
			return;
		}
	}

	// Add new entry by extending environ
	environ = ft_array_append(environ, entry);
}
