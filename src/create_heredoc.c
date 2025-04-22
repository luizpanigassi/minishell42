/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   create_heredoc.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: luinasci <luinasci@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/17 09:59:44 by jcologne          #+#    #+#             */
/*   Updated: 2025/04/22 17:05:56 by luinasci         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <minishell.h>

/**
 * @brief Creates a temporary heredoc file.
 * @param delimiter Heredoc termination string.
 * @return File descriptor of the heredoc temp file.
 * @note Automatically deletes the temp file after opening.
 */
int create_heredoc(const char *delimiter)
{
	int pipefd[2];
	pid_t pid;
	int status;
	char *line;
	int quoted_delimiter;
	char *expanded;

	if (pipe(pipefd) < 0)
		return (-1);
	pid = fork();
	if (pid == -1)
	{
		close(pipefd[0]);
		close(pipefd[1]);
		return (-1);
	}
	if (pid == 0) // Child process
	{
		close(pipefd[0]); // Close read end in child
		signal(SIGINT, SIG_DFL);
		signal(SIGQUIT, SIG_IGN);
		quoted_delimiter = (delimiter[0] == '\'' || delimiter[0] == '"');
		while (1)
		{
			line = readline("> ");
			if (!line) // Handle Ctrl+D
			{
				write(STDOUT_FILENO, "\n", 1);
				break;
			}
			if (strcmp(line, delimiter) == 0) // End of heredoc
			{
				free(line);
				break;
			}
			if (!quoted_delimiter)
			{
				expanded = expand_variables(line);
				write(pipefd[1], expanded, strlen(expanded));
				free(expanded);
			}
			else
				write(pipefd[1], line, strlen(line));
			write(pipefd[1], "\n", 1); // Add newline after each line
			free(line);
		}
		close(pipefd[1]); // Close write end in child
		exit(0);
	}
	else // Parent process
	{
		close(pipefd[1]); // Close write end in parent
		waitpid(pid, &status, 0);
		if (WIFSIGNALED(status) && WTERMSIG(status) == SIGINT)
		{
			close(pipefd[0]);
			return (-1); // Return error if interrupted
		}
		return (pipefd[0]); // Return read end of the pipe
	}
}
