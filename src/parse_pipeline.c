/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse_pipeline.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: luinasci <luinasci@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/23 15:44:42 by jcologne          #+#    #+#             */
/*   Updated: 2025/04/28 19:32:49 by luinasci         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/**
 * @brief Converts raw input into command structures.
 * @param p Initialized parser state.
 * @return Parsed command structure or NULL on error.
 * @note Handles quotes, redirections, and command chaining.
 */
t_cmd *parse_pipeline(t_parse *p)
{
	t_cmd *head;
	t_cmd **curr;
	t_cmd *cmd;
	char *error_token;

	head = NULL;
	curr = &head;
	next_token(p);

	// empty input or immediate pipe
	if (p->token_type == T_PIPE || p->token_type == T_SEMICOLON)
	{
		syntax_error(p->token_value ? p->token_value : "newline");
		return NULL;
	}
	while (1)
	{
		cmd = parse_args(p);
		// parse_args failed
		if (p->syntax_error)
		{
			free_cmd(head);
			return NULL;
		}
		// valid command
		if (cmd)
		{
			*curr = cmd;
			curr = &cmd->next;
		}
		// pipeline termination conditions
		if (!cmd || p->token_type != T_PIPE)
			break;
		next_token(p);
		// consecutive pipes
		error_token = p->token_value;
		if (p->token_type == T_EOF)
			error_token = "newline";
		else if (p->token_type == T_PIPE || p->token_type == T_SEMICOLON)
			error_token = p->token_value;
		if (p->token_type == T_PIPE || p->token_type == T_SEMICOLON || p->token_type == T_EOF)
		{
			syntax_error(error_token);
			free_cmd(head);
			return NULL;
		}
	}
	// syntax check after pipeline parsing
	if (p->syntax_error)
	{
		free_cmd(head);
		return NULL;
	}
	return head;
}
