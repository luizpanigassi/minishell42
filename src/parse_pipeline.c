/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse_pipeline.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: luinasci <luinasci@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/23 15:44:42 by jcologne          #+#    #+#             */
/*   Updated: 2025/05/07 19:46:00 by luinasci         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/**
 * @brief Checks if the current token is a pipe or semicolon.
 * @param p Parser state containing the current token and its value.
 * @return 1 if the token is a pipe or semicolon, 0 otherwise.
 * @note Reports a syntax error if the token is invalid.
 */
int	is_pipe_or_semicolon(t_parse *p)
{
	if (p->token_type == T_PIPE || p->token_type == T_SEMICOLON)
	{
		if (p->token_value)
			syntax_error(p->token_value);
		else
			syntax_error("newline");
		free(p->token_value);
		p->token_value = NULL;
		return (1);
	}
	return (0);
}

/**
 * @brief Handles the result of parsing a command.
 * @param p Parser state containing the current token and its value.
 * @param head Pointer to the head of the command list.
 * @param cmd The parsed command to handle.
 * @param curr Pointer to the current position in the command list.
 * @return 1 if a syntax error occurred, 0 otherwise.
 * @note Frees resources and updates the command list as needed.
 */
int	handle_cmd_result(t_parse *p, t_cmd **head,
			t_cmd *cmd, t_cmd ***curr)
{
	if (p->syntax_error)
	{
		free(p->token_value);
		p->token_value = NULL;
		free_cmd(*head);
		return (1);
	}
	if (cmd)
	{
		**curr = cmd;
		*curr = &cmd->next;
	}
	return (0);
}

/**
 * @brief Checks for syntax errors related to pipes or semicolons.
 * @param p Parser state containing the current token and its value.
 * @param head Pointer to the head of the command list for cleanup.
 * @return 1 if a syntax error is detected, 0 otherwise.
 * @note Reports errors for unexpected pipes, semicolons, or EOF tokens.
 */
int	check_pipe_error(t_parse *p, t_cmd *head)
{
	char	*err_token;

	err_token = NULL;
	if (p->token_type == T_EOF)
		err_token = "newline";
	if (p->token_type == T_PIPE || p->token_type == T_SEMICOLON)
		err_token = p->token_value;
	if (p->token_type == T_PIPE
		|| p->token_type == T_SEMICOLON
		|| p->token_type == T_EOF)
	{
		if (err_token)
			syntax_error(err_token);
		else
			syntax_error("newline");
		free(p->token_value);
		p->token_value = NULL;
		free_cmd(head);
		return (1);
	}
	return (0);
}

/**
 * @brief Checks for final syntax errors after parsing.
 * @param p Parser state containing the current token and its value.
 * @param head Pointer to the head of the command list for cleanup.
 * @return 1 if a syntax error is detected, 0 otherwise.
 * @note Frees resources if a syntax error is present.
 */
int	check_final_error(t_parse *p, t_cmd *head)
{
	if (p->syntax_error)
	{
		free(p->token_value);
		p->token_value = NULL;
		free_cmd(head);
		return (1);
	}
	return (0);
}

/**
 * @brief Parses a pipeline of commands.
 * @param p Parser state containing the tokens to parse.
 * @return Pointer to the head of the parsed command list, or NULL on error.
 * @note Handles syntax errors, pipes, and semicolons during parsing.
 */
t_cmd	*parse_pipeline(t_parse *p)
{
	t_cmd	*head;
	t_cmd	**curr;
	t_cmd	*cmd;

	head = NULL;
	curr = &head;
	next_token(p);
	if (is_pipe_or_semicolon(p))
		return (NULL);
	while (1)
	{
		cmd = parse_args(p);
		if (handle_cmd_result(p, &head, cmd, &curr))
			return (NULL);
		if (!cmd || p->token_type != T_PIPE)
			break ;
		next_token(p);
		if (check_pipe_error(p, head))
			return (NULL);
	}
	if (check_final_error(p, head))
		return (NULL);
	return (head);
}
