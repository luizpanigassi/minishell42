/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse_args.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jcologne <jcologne@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/23 14:36:26 by jcologne          #+#    #+#             */
/*   Updated: 2025/04/23 15:04:41 by jcologne         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/* Process a single argument and add it to the args list */
static void	process_argument(t_parse *p, t_list **args)
{
	t_arg	*arg;

	arg = malloc(sizeof(t_arg));
	arg->value = ft_strdup(p->token_value);
	arg->type = p->token_type;
	ft_lstadd_back(args, ft_lstnew(arg));
}

/* Handle redirection errors and return appropriate error message */
static t_redir	*handle_redir_error(t_parse *p, t_list **args, t_redir *redirs)
{
	ft_putstr_fd("minishell: syntax error near unexpected token `",
		STDERR_FILENO);
	if (p->token_type == T_EOF)
		ft_putstr_fd("newline", STDERR_FILENO);
	else if (p->token_value)
		ft_putstr_fd(p->token_value, STDERR_FILENO);
	else
		ft_putstr_fd(" ", STDERR_FILENO);
	ft_putstr_fd("'\n", STDERR_FILENO);
	ft_lstclear(args, free_arg);
	free_redirections(redirs);
	return (NULL);
}

/* Process a redirection and add it to the redirection list */
static t_redir	*process_redirection(t_parse *p, t_list **args, t_redir *redirs)
{
	t_redir	*redir;

	redir = malloc(sizeof(t_redir));
	if (!redir)
	{
		ft_lstclear(args, free_arg);
		free_redirections(redirs);
		return (NULL);
	}
	redir->fd = p->redir_fd;
	redir->type = p->token_type;
	next_token(p);
	if (p->token_type != T_WORD && p->token_type != T_SINGLE_QUOTED
		&& p->token_type != T_DOUBLE_QUOTED)
	{
		free(redir);
		return (handle_redir_error(p, args, redirs));
	}
	p->redir_fd = 0;
	redir->filename = ft_strdup(p->token_value);
	redir->next = NULL;
	return (redir);
}

/* Create the final command structure */
static t_cmd	*create_command(t_list *args, t_redir *redirs)
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

/* Main parse_args function, now using helper functions */
t_cmd	*parse_args(t_parse *p)
{
	t_list	*args;
	t_redir	*redirs;
	t_redir	**redir_tail;
	t_redir	*new_redir;

	args = NULL;
	redirs = NULL;
	redir_tail = &redirs;
	while (p->token_type != T_EOF && p->token_type != T_PIPE
		&& p->token_type != T_SEMICOLON)
	{
		if (p->token_type == T_WORD || p->token_type == T_SINGLE_QUOTED
			|| p->token_type == T_DOUBLE_QUOTED)
			process_argument(p, &args);
		else if (is_redirection(p->token_type))
		{
			new_redir = process_redirection(p, &args, redirs);
			if (!new_redir)
				return (NULL);
			*redir_tail = new_redir;
			redir_tail = &new_redir->next;
		}
		next_token(p);
	}
	return (create_command(args, redirs));
}
