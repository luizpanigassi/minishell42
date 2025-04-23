/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse_pipeline.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jcologne <jcologne@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/23 15:44:42 by jcologne          #+#    #+#             */
/*   Updated: 2025/04/23 15:58:53 by jcologne         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static int	is_invalid_start_token(t_parse *p)
{
	if (p->token_type == T_PIPE || p->token_type == T_SEMICOLON)
	{
		syntax_error(p->token_value);
		return (1);
	}
	return (0);
}

static int	handle_pipe_error(t_parse *p, t_cmd *head)
{
	char	*error_token;

	error_token = p->token_value;
	if (p->token_type == T_EOF)
		error_token = "newline";
	else if (p->token_type == T_PIPE || p->token_type == T_SEMICOLON)
		error_token = p->token_value;
	if (p->token_type == T_PIPE || p->token_type == T_SEMICOLON
		|| p->token_type == T_EOF)
	{
		syntax_error(error_token);
		free_cmd(head);
		return (1);
	}
	return (0);
}

static int	parse_command(t_parse *p, t_cmd **curr)
{
	t_cmd	*cmd;

	cmd = parse_args(p);
	if (!cmd)
		return (0);
	*curr = cmd;
	return (1);
}

t_cmd	*parse_pipeline(t_parse *p)
{
	t_cmd	*head;
	t_cmd	**curr;

	head = NULL;
	curr = &head;
	next_token(p);
	if (is_invalid_start_token(p))
		return (NULL);
	while (1)
	{
		if (!parse_command(p, curr))
			break ;
		curr = &(*curr)->next;
		if (p->token_type != T_PIPE)
			break ;
		next_token(p);
		if (handle_pipe_error(p, head))
			return (NULL);
	}
	return (head);
}
