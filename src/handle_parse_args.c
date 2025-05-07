/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   handle_parse_args.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: luinasci <luinasci@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/07 16:26:48 by luinasci          #+#    #+#             */
/*   Updated: 2025/05/07 19:44:02 by luinasci         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/**
 * @brief Handles redirection syntax errors.
 * @param p Parser state.
 * @param args Argument list to clear.
 * @param redirs Redirection list to clear.
 * @return Always returns NULL.
 * @note Performs full cleanup on syntax error detection.
 */
t_redir	*handle_redir_error(t_parse *p, t_list **args, t_redir *redirs)
{
	ft_putstr_fd("minishell: syntax error near unexpected token `",
		STDERR_FILENO);
	if (p->token_type == T_EOF)
		ft_putstr_fd("newline", STDERR_FILENO);
	else
		ft_putstr_fd(p->token_value, STDERR_FILENO);
	ft_putstr_fd("'\n", STDERR_FILENO);
	ft_lstclear(args, free_arg);
	free_redirections(redirs);
	p->syntax_error = 1;
	return (NULL);
}

/**
 * @brief Handles an argument token during parsing.
 * @param p Parser state containing the current token and its value.
 * @param args Pointer to the list of arguments being built.
 * @param redirs Pointer to the list of redirections for cleanup in
 * case of error.
 * @return 1 on success, 0 on failure.
 * @note Adds the argument to the list or performs cleanup if the token
 * is invalid.
 */
int	handle_argument_token(t_parse *p, t_list **args, t_redir *redirs)
{
	if (!p->token_value)
	{
		ft_lstclear(args, free_arg);
		free_redirections(redirs);
		return (0);
	}
	process_argument(p, args);
	return (1);
}

/**
 * @brief Handles a redirection token during parsing.
 * @param p Parser state containing the current token and its value.
 * @param args Pointer to the list of arguments for cleanup in case of error.
 * @param redir_tail Pointer to the tail of the redirection list for appending.
 * @param redirs Pointer to the list of redirections for cleanup in
 * case of error.
 * @return 1 on success, 0 on failure.
 * @note Processes the redirection and appends it to the redirection list.
 */
int	handle_redirection_token(t_parse *p, t_list **args,
	t_redir **redir_tail, t_redir *redirs)
{
	t_redir	*new_redir;

	new_redir = process_redirection(p, args, redirs);
	if (!new_redir)
		return (0);
	*redir_tail = new_redir;
	redir_tail = &new_redir->next;
	return (1);
}

/**
 * @brief Handles syntax errors and performs cleanup.
 * @param p Parser state.
 * @param args Argument list to clear.
 * @param redirs Redirection list to clear.
 * @param error_message Error message to display.
 * @return Always returns NULL.
 */
t_cmd	*handle_syntax_error(t_parse *p, t_list *args, t_redir *redirs,
	char *error_message)
{
	if (error_message)
		ft_putstr_fd(error_message, STDERR_FILENO);
	p->syntax_error = 1;
	ft_lstclear(&args, free_arg);
	free_redirections(redirs);
	return (NULL);
}
