/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   handle_special.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: luinasci <luinasci@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/23 14:06:27 by jcologne          #+#    #+#             */
/*   Updated: 2025/04/29 18:35:14 by luinasci         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/**
 * @brief Assigns token values during parsing.
 * @param p Parser state.
 * @param t Token type to assign.
 * @param v Token value string.
 * @param n Whether to advance character position.
 * @note Manages parser state transitions for special tokens.
 */
void	assign_value(t_parse *p, enum e_token t, char *v, int n)
{
	p->token_type = t;
	p->token_value = ft_strdup(v);
	if (n)
		next_char(p);
}

/**
 * @brief Processes file descriptor prefix in redirections.
 * @param p Parser state.
 * @note Extracts leading digits for numbered file descriptors.
 */
void	handle_file_descriptor(t_parse *p)
{
	int		fd;
	char	*fd_str;

	fd = 0;
	if (ft_isdigit(p->curr_char))
	{
		fd_str = parse_fd(p);
		fd = ft_atoi(fd_str);
		free(fd_str);
		p->redir_fd = fd;
	}
}

/**
 * @brief Handles output redirection tokens (> and >>).
 * @param p Parser state.
 * @note Determines between truncate (>) and append (>>) modes.
 */
void	handle_output_redirection(t_parse *p)
{
	next_char(p);
	if (p->curr_char == '>')
		assign_value(p, T_APPEND, ">>", 1);
	else
		assign_value(p, T_REDIR_OUT, ">", 0);
}

/**
 * @brief Handles input redirection tokens (< and <<).
 * @param p Parser state.
 * @note Distinguishes between regular input and heredoc redirection.
 */
void	handle_input_redirection(t_parse *p)
{
	next_char(p);
	if (p->curr_char == '<')
		assign_value(p, T_HEREDOC, "<<", 1);
	else
		assign_value(p, T_REDIR_IN, "<", 0);
}

/**
 * @brief Processes redirection tokens during parsing.
 * @param p Parser state structure.
 * @note Handles >, >>, <, << operators and file descriptors.
 */
void	handle_special(t_parse *p)
{
	handle_file_descriptor(p);
	if (p->curr_char == '|')
		assign_value(p, T_PIPE, "|", 1);
	else if (p->curr_char == '>')
		handle_output_redirection(p);
	else if (p->curr_char == '<')
		handle_input_redirection(p);
}
