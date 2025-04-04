/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   input.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: luinasci <luinasci@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/27 18:13:53 by luinasci          #+#    #+#             */
/*   Updated: 2025/03/27 18:20:55 by luinasci         ###   ########.fr       */
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

/*
** Finds the full path of a command using PATH environment variable
** @param cmd Command name to locate
** @return Full path if found, NULL otherwise
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
		if(access(cmd, X_OK) == 0)
			return (ft_strdup(cmd));
		return (NULL);
	}
	// Get PATH environment variable
	char *path_env = getenv("PATH");
	if(!path_env)
		return (NULL);
	// Split PATH into individual directories
	paths = ft_split(path_env, ':');
	if(!paths)
		return (NULL);

	i = -1;
	while (paths[++i])
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
		free (full_path);
	}
	ft_free_array(paths);
	return (NULL);
}
