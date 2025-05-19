/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   handle_redirect.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: luinasci <luinasci@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/07 17:12:52 by luinasci          #+#    #+#             */
/*   Updated: 2025/05/07 17:22:09 by luinasci         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/**
 * @brief Opens a file descriptor based on the redirection type.
 * @param redir Redirection structure containing type and filename.
 * @return File descriptor on success, -1 on error.
 */
int	open_redirection_fd(t_redir *redir)
{
	if (redir->type == T_HEREDOC)
		return (redir->fd);
	else if (redir->type == T_REDIR_IN)
		return (open(redir->filename, O_RDONLY));
	else if (redir->type == T_REDIR_OUT)
		return (open(redir->filename, O_WRONLY | O_CREAT | O_TRUNC, 0644));
	else if (redir->type == T_APPEND)
		return (open(redir->filename, O_WRONLY | O_CREAT | O_APPEND, 0644));
	return (-1);
}

/**
 * @brief Duplicates a file descriptor to a target and handles errors.
 * @param fd File descriptor to duplicate.
 * @param target Target file descriptor.
 * @return 0 on success, -1 on error.
 */
int	duplicate_fd(int fd, int target)
{
	if (dup2(fd, target) == -1)
	{
		close(fd);
		perror("minishell");
		return (-1);
	}
	close(fd);
	return (0);
}

/**
 * @brief Processes a single redirection by opening and duplicating the
 * file descriptor.
 * @param redir Redirection structure containing type and filename.
 * @return 0 on success, -1 on error.
 */
int	handle_single_redirection(t_redir *redir)
{
	int	fd;

	fd = open_redirection_fd(redir);
	if (fd == -1)
	{
		perror("minishell");
		set_exit_status(1);
		exit(EXIT_FAILURE);
	}
	if (redir->type == T_REDIR_IN || redir->type == T_HEREDOC)
	{
		if (duplicate_fd(fd, STDIN_FILENO) == -1)
			return (-1);
	}
	else
	{
		if (duplicate_fd(fd, STDOUT_FILENO) == -1)
			return (-1);
	}
	return (0);
}

/**
 * @brief Handles file redirections for a command.
 * @param redirections Redirection specification list.
 * @return 0 on success, -1 on error.
 */
int	handle_file_redirections(t_redir *redirections)
{
	t_redir	*current;

	current = redirections;
	while (current)
	{
		if (handle_single_redirection(current) == -1)
			return (-1);
		current = current->next;
	}
	return (0);
}

/**
 * @brief Applies redirection operations for command.
 * @param pipe_in Input file descriptor.
 * @param pipe_out Output file descriptor.
 * @param redirections Redirection specification list.
 * @return 0 on success, -1 on error.
 * @note Handles heredoc, truncate, and append modes.
 */
int	handle_redirections(int pipe_in, int pipe_out, t_redir *redirections)
{
	if (pipe_in != -1)
	{
		if (dup2(pipe_in, STDIN_FILENO) == -1)
			return (perror("minishell"), -1);
		close(pipe_in);
	}
	if (pipe_out != -1)
	{
		if (dup2(pipe_out, STDOUT_FILENO) == -1)
			return (perror("minishell"), -1);
		close(pipe_out);
	}
	return (handle_file_redirections(redirections));
}
