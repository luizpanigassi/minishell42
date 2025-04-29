/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   create_heredoc.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: luinasci <luinasci@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/17 09:59:44 by jcologne          #+#    #+#             */
/*   Updated: 2025/04/29 19:10:18 by luinasci         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <minishell.h>

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

int	is_quoted_delimiter(const char *delimiter)
{
	return (delimiter[0] == '\'' || delimiter[0] == '"');
}

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

int	handle_parent_process(pid_t pid, int read_fd)
{
	int	status;

	waitpid(pid, &status, 0);
	if (WIFSIGNALED(status) && WTERMSIG(status) == SIGINT)
	{
		close(read_fd);
		return (-1);
	}
	return (read_fd);
}

int	create_heredoc(const char *delimiter)
{
	int		pipefd[2];
	pid_t	pid;

	if (pipe(pipefd) < 0)
		return (-1);
	pid = fork();
	if (pid == -1)
	{
		close(pipefd[0]);
		close(pipefd[1]);
		return (-1);
	}
	if (pid == 0)
	{
		close(pipefd[0]);
		handle_child_process(pipefd[1], delimiter);
	}
	else
	{
		close(pipefd[1]);
		return (handle_parent_process(pid, pipefd[0]));
	}
	return (0);
}
