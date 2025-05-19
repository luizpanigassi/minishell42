/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   handle_heredoc.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: luinasci <luinasci@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/08 16:32:07 by luinasci          #+#    #+#             */
/*   Updated: 2025/05/08 16:40:06 by luinasci         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <minishell.h>

/**
 * @brief Writes a line to the pipe, expanding variables if necessary.
 * @param write_fd File descriptor for the write end of the pipe.
 * @param line The line to write to the pipe.
 * @param quoted_delimiter Flag indicating if the delimiter is quoted.
 * @note If the delimiter is not quoted, variables in the line are expanded.
 */
void	write_line_to_pipe(int write_fd, const char *line, int quoted_delimiter)
{
	char	*expanded;

	if (!quoted_delimiter)
	{
		expanded = expand_variables(line);
		write(write_fd, expanded, strlen(expanded));
		free(expanded);
	}
	else
		write(write_fd, line, strlen(line));
	write(write_fd, "\n", 1);
}

/**
 * @brief Checks if the given delimiter is quoted.
 * @param delimiter The delimiter string to check.
 * @return 1 if the delimiter is quoted, 0 otherwise.
 * @note A delimiter is considered quoted if it starts with a single
 * or double quote.
 */
int	is_quoted_delimiter(const char *delimiter)
{
	return (delimiter[0] == '\'' || delimiter[0] == '"');
}

/**
 * @brief Handles the child process for the heredoc creation.
 * @param write_fd File descriptor for the write end of the pipe.
 * @param delimiter The delimiter string to terminate the heredoc.
 * @note Reads lines from the user until the delimiter is encountered.
 *       Expands variables unless the delimiter is quoted.
 */
void	handle_child_process(int write_fd, const char *delimiter)
{
	char	*line;
	int		quoted_delimiter;

	quoted_delimiter = is_quoted_delimiter(delimiter);
	signal(SIGINT, SIG_DFL);
	signal(SIGQUIT, SIG_IGN);
	while (1)
	{
		line = readline("> ");
		if (!line)
		{
			write(STDOUT_FILENO, "\n", 1);
			break ;
		}
		if (strcmp(line, delimiter) == 0)
			break ;
		write_line_to_pipe(write_fd, line, quoted_delimiter);
		free(line);
	}
	close(write_fd);
	exit(0);
}

/**
 * @brief Handles the parent process after forking for heredoc.
 * @param pid The process ID of the child process.
 * @param read_fd File descriptor for the read end of the pipe.
 * @return The read file descriptor on success, -1 if interrupted by SIGINT.
 * @note Waits for the child process to finish and checks for signals.
 */
int	handle_parent_process(pid_t pid, int read_fd, int *was_signaled)
{
	int	status;

	*was_signaled = 0;
	waitpid(pid, &status, 0);
	if (WIFSIGNALED(status) && WTERMSIG(status) == SIGINT)
	{
		*was_signaled = 1;
		close(read_fd);
		return (-1);
	}
	return (read_fd);
}
