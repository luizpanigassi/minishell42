/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   create_heredoc.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jcologne <jcologne@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/17 09:59:44 by jcologne          #+#    #+#             */
/*   Updated: 2025/04/17 11:33:41 by jcologne         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <minishell.h>

/**
 * @brief Creates a temporary heredoc file.
 * @param delimiter Heredoc termination string.
 * @return File descriptor of the heredoc temp file.
 * @note Automatically deletes the temp file after opening.
 */
int	create_heredoc(const char *delimiter)
{
	int		pipefd[2];
	pid_t	pid;
	int		status;
	char	*line;
	int		quoted_delimiter;
	char	*expanded;

	if (pipe(pipefd))
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
		signal(SIGINT, SIG_DFL);
		signal(SIGQUIT, SIG_IGN);
		quoted_delimiter = (delimiter[0] == '\'' || delimiter[0] == '"');
		while (1)
		{
			line = readline("> ");
			if (!line)
				break ;
			if (strcmp(line, delimiter) == 0)
			{
				free(line);
				break ;
			}
			if (!quoted_delimiter)
			{
				expanded = expand_variables(line);
				write(pipefd[1], expanded, strlen(expanded));
				free(expanded);
			}
			else
			{
				write(pipefd[1], line, strlen(line));
			}
			write(pipefd[1], "\n", 1);
			free(line);
		}
		close(pipefd[1]);
		exit(0);
	}
	else
	{
		close(pipefd[1]);
		waitpid(pid, &status, 0);
		if (WIFSIGNALED(status) && WTERMSIG(status) == SIGINT)
		{
			close(pipefd[0]);
			rl_replace_line("", 0);
			rl_on_new_line();
			ft_putstr_fd("\n", STDOUT_FILENO);
			set_exit_status(130);
			return (-1);
		}
		return (pipefd[0]);
	}
}
