/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_cmd_path.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jcologne <jcologne@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/17 14:53:18 by jcologne          #+#    #+#             */
/*   Updated: 2025/04/17 17:47:32 by jcologne         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static char	*check_path_for_cmd(const char *dir_path, const char *cmd)
{
	char	*dir_slash;
	char	*full_path;

	dir_slash = ft_strjoin(dir_path, "/");
	if (!dir_slash)
		return (NULL);
	full_path = ft_strjoin(dir_slash, cmd);
	free(dir_slash);
	if (!full_path)
		return (NULL);
	if (access(full_path, X_OK) == 0)
		return (full_path);
	free(full_path);
	return (NULL);
}

static char	**get_path_directories(void)
{
	char	*path_env;
	char	**paths;

	path_env = getenv("PATH");
	if (!path_env)
		return (NULL);
	paths = ft_split(path_env, ':');
	if (!paths)
		return (NULL);
	return (paths);
}

static char	*check_direct_path(char *cmd)
{
	if (ft_strchr(cmd, '/') != NULL)
	{
		if (access(cmd, F_OK) == -1)
			return (NULL);
		if (access(cmd, X_OK) == -1)
		{
			g_exit_status = PERM_DENIED;
			return (NULL);
		}
		return (ft_strdup(cmd));
	}
	return (NULL);
}

/**
 * @brief Locates the full path of a command using PATH.
 * @param cmd Command name (e.g., "ls").
 * @return Allocated full path (e.g., "/bin/ls") or NULL if not found.
 * Caller must free the returned string.
 */
char	*get_cmd_path(char *cmd)
{
	char	**paths;
	char	*full_path;
	int		i;

	full_path = check_direct_path(cmd);
	if (full_path)
		return (full_path);
	paths = get_path_directories();
	if (!paths)
		return (NULL);
	i = 0;
	while (paths[i])
	{
		full_path = check_path_for_cmd(paths[i], cmd);
		if (full_path)
		{
			ft_free_array(paths);
			return (full_path);
		}
		i++;
	}
	ft_free_array(paths);
	return (NULL);
}
