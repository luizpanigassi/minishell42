/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse_args.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: luinasci <luinasci@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/23 14:36:26 by jcologne          #+#    #+#             */
/*   Updated: 2025/05/07 16:27:50 by luinasci         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/**
 * @brief Adds parsed argument to command structure.
 * @param p Parser state.
 * @param args Argument list accumulator.
 * @note Handles quoted and unquoted argument types.
 */
void	process_argument(t_parse *p, t_list **args)
{
	t_arg	*arg;

	arg = malloc(sizeof(t_arg));
	arg->value = ft_strdup(p->token_value);
	arg->type = p->token_type;
	ft_lstadd_back(args, ft_lstnew(arg));
}

/**
 * @brief Processes redirection tokens during parsing.
 * @param p Parser state.
 * @param args Argument list accumulator.
 * @param redirs Redirection list accumulator.
 * @return New redirection node or NULL on error.
 * @note Handles syntax validation for redirection targets.
 */
t_redir	*process_redirection(t_parse *p, t_list **args, t_redir *redirs)
{
	t_redir	*redir;

	redir = malloc(sizeof(t_redir));
	if (!redir)
		return (NULL);
	redir->fd = p->redir_fd;
	redir->type = p->token_type;
	next_token(p);
	if (!p->token_value || (p->token_type != T_WORD && p->token_type
			!= T_SINGLE_QUOTED && p->token_type != T_DOUBLE_QUOTED))
		return (free(redir), handle_redir_error(p, args, redirs));
	p->redir_fd = 0;
	redir->filename = ft_strdup(p->token_value);
	redir->next = NULL;
	if (p->syntax_error)
	{
		free(redir);
		ft_lstclear(args, free_arg);
		free_redirections(redirs);
		p->syntax_error = 1;
		return (NULL);
	}
	return (redir);
}

/**
 * @brief Constructs final command structure.
 * @param args List of parsed arguments.
 * @param redirs List of redirections.
 * @return Allocated command structure.
 * @note Transforms linked lists into arrays for execution.
 */
t_cmd	*create_command(t_list *args, t_redir *redirs)
{
	t_cmd	*cmd;

	cmd = malloc(sizeof(t_cmd));
	if (!cmd)
	{
		ft_lstclear(&args, free_arg);
		free_redirections(redirs);
		return (NULL);
	}
	cmd->args = build_expanded_args(args);
	cmd->redirections = redirs;
	cmd->next = NULL;
	ft_lstclear(&args, free_arg);
	return (cmd);
}

/**
 * @brief Struct to encapsulate token processing state.
 */

/**
 * @brief Processes tokens to build arguments and redirections.
 * @param p Parser state.
 * @param state Token processing state (encapsulates args, redirs, etc.).
 * @return 1 on success, 0 on error.
 */
int	process_tokens(t_parse *p, t_token_state *state)
{
	t_redir	*new_redir;

	while (p->token_type != T_EOF && p->token_type
		!= T_PIPE && p->token_type != T_SEMICOLON)
	{
		if (p->token_type == T_WORD || p->token_type == T_SINGLE_QUOTED
			|| p->token_type == T_DOUBLE_QUOTED)
		{
			if (!handle_argument_token(p, state->args, *(state->redirs)))
				return (0);
			*(state->has_args) = 1;
		}
		else if (is_redirection(p->token_type))
		{
			new_redir = process_redirection(p, state->args, *(state->redirs));
			if (!new_redir)
				return (0);
			**(state->redir_tail) = new_redir;
			*(state->redir_tail) = &new_redir->next;
		}
		next_token(p);
	}
	return (1);
}

/**
 * @brief Core command argument parser.
 * @param p Parser state.
 * @return Complete command structure.
 * @note Handles words, redirections, and syntax error detection.
 */
t_cmd	*parse_args(t_parse *p)
{
	t_list			*args;
	t_redir			*redirs;
	t_redir			**redir_tail;
	int				has_args;
	t_token_state	state;

	has_args = 0;
	redir_tail = &redirs;
	redirs = NULL;
	args = NULL;
	state.args = &args;
	state.redirs = &redirs;
	state.redir_tail = &redir_tail;
	state.has_args = &has_args;
	if (!process_tokens(p, &state))
		return (handle_syntax_error(p, args, redirs, NULL));
	if (!has_args && redirs)
		return (handle_syntax_error(p, args, redirs,
				"minishell: syntax error near unexpected token '>'\n"));
	return (create_command(args, redirs));
}
