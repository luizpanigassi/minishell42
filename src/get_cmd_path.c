/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_cmd_path.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: luinasci <luinasci@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/17 14:53:18 by jcologne          #+#    #+#             */
/*   Updated: 2025/05/08 16:47:50 by luinasci         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/**
 * @brief Constructs full command path from directory and command name.
 * @param dir_path Directory path component.
 * @param cmd Command name component.
 * @return Full path string or NULL.
 * @note Checks execute permissions on constructed path.
 */
char	*check_path_for_cmd(const char *dir_path, const char *cmd)
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

/**
 * @brief Extracts PATH directories into array.
 * @return Null-terminated array of directory strings.
 * @note Splits PATH environment variable using colon delimiter.
 */
char	**get_path_directories(void)
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

/**
 * @brief Checks if command contains direct path specification.
 * @param cmd Command string to check.
 * @return Full path if valid direct path, NULL otherwise.
 * @note Verifies existence and permissions of path.
 */
char	*check_direct_path(char *cmd)
{
	struct stat	path_stat;

	if (ft_strchr(cmd, '/') != NULL)
	{
		if (access(cmd, F_OK) == -1)
			return (NULL);
		if (stat(cmd, &path_stat) == -1 || !S_ISREG(path_stat.st_mode))
		{
			g_exit_status = PERM_DENIED;
			return (NULL);
		}
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

/**
 * @brief Executes a single command with I/O redirection.
 * @param cmd Command structure to execute.
 * @param pipe_in Input file descriptor (or -1).
 * @param pipe_out Output file descriptor (or -1).
 */
void	execute_command(t_cmd *cmd, int pipe_in, int pipe_out)
{
	pid_t				pid;
	struct sigaction	sa;
	struct sigaction	old_sa;

	sa.sa_handler = SIG_IGN;
	sa.sa_flags = SA_RESTART;
	sigemptyset(&sa.sa_mask);
	if (sigaction(SIGINT, &sa, &old_sa) == -1)
	{
		perror("minishell: sigaction");
		return ;
	}
	pid = fork();
	if (pid == 0)
	{
		setup_and_execute_child(cmd, pipe_in, pipe_out);
	}
	else if (pid < 0)
		perror("fork");
	else
		handle_child_exit(pid, &old_sa);
}
