/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_cd.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: luinasci <luinasci@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/17 10:38:13 by jcologne          #+#    #+#             */
/*   Updated: 2025/05/07 19:38:46 by luinasci         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/**
 * @brief Changes the current working directory.
 * @param args Command arguments (args[1] is target directory).
 * @return 0 on success, 1 on failure.
 * @note Updates PWD and OLDPWD environment variables.
 */
int	exec_cd(char **args)
{
	char	*oldpwd;
	char	*newpwd;

	oldpwd = getcwd(NULL, 0);
	if (!oldpwd)
		return (perror("cd"), 1);
	if (handle_cd_arguments(args, oldpwd) != 0)
		return (1);
	newpwd = getcwd(NULL, 0);
	if (!newpwd)
	{
		free(oldpwd);
		return (perror("cd"), 1);
	}
	update_env_var("OLDPWD", oldpwd);
	free(oldpwd);
	update_env_var("PWD", newpwd);
	free(newpwd);
	return (0);
}

/**
 * @brief Resolves the target path for the cd command.
 * @param args Command arguments (args[1] is the target directory).
 * @param oldpwd Current working directory before the change.
 * @return Resolved path as a string, or NULL on error.
 * @note Handles special cases for ~ (HOME), - (OLDPWD), and empty arguments.
 *       Prints an error message if HOME or OLDPWD is not set.
 */
char	*resolve_cd_path(char **args, char *oldpwd)
{
	char	*path;

	if (!args[1] || ft_strcmp(args[1], "~") == 0)
	{
		path = getenv("HOME");
		if (!path)
		{
			ft_putstr_fd("minishell: cd: HOME not set\n", STDERR_FILENO);
			free(oldpwd);
			return (NULL);
		}
	}
	else if (ft_strcmp(args[1], "-") == 0)
	{
		path = getenv("OLDPWD");
		if (!path)
		{
			ft_putstr_fd("minishell: cd: OLDPWD not set\n", STDERR_FILENO);
			free(oldpwd);
			return (NULL);
		}
	}
	else
		path = args[1];
	return (path);
}

/**
 * @brief Handles argument validation and path resolution for cd command.
 * @param args Command arguments array.
 * @param oldpwd Current working directory before change.
 * @return 0 on success, 1 on error.
 * @note Manages ~, -, and empty argument special cases.
 */

int	handle_cd_arguments(char **args, char *oldpwd)
{
	char	*path;

	if (args[1] && args[2])
	{
		ft_putstr_fd("minishell: cd: too many arguments\n", STDERR_FILENO);
		free(oldpwd);
		set_exit_status(1);
		return (1);
	}
	path = resolve_cd_path(args, oldpwd);
	if (!path)
		return (1);
	if (chdir(path) != 0)
	{
		perror("cd");
		set_exit_status(1);
		free(oldpwd);
		return (1);
	}
	return (0);
}
