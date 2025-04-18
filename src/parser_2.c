/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_2.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: luinasci <luinasci@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/17 09:46:38 by jcologne          #+#    #+#             */
/*   Updated: 2025/04/18 16:05:23 by luinasci         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/*
** Parses command arguments and redirections
** @param p Parser structure
** @return Command structure with arguments and redirections
*/
t_cmd	*parse_args(t_parse *p)
{
	t_list	*args;
	t_redir	*redirs;
	t_redir	**redir_tail;
	t_arg	*arg;
	t_redir	*redir;
	t_cmd	*cmd;

	args = NULL;
	redirs = NULL;
	redir_tail = &redirs;
	while (p->token_type != T_EOF && p->token_type != T_PIPE && p->token_type != T_SEMICOLON)
	{
		if (p->token_type == T_WORD || p->token_type == T_SINGLE_QUOTED || p->token_type == T_DOUBLE_QUOTED)
		{
			arg = malloc(sizeof(t_arg));
			arg->value = ft_strdup(p->token_value);
			arg->type = p->token_type;
			ft_lstadd_back(&args, ft_lstnew(arg));
		}
		else if (is_redirection(p->token_type))
		{
			redir = malloc(sizeof(t_redir));
			redir->fd = p->redir_fd;
			if (!redir)
			{
				ft_lstclear(&args, free_arg);
				return (NULL);
			}
			redir->type = p->token_type;
			next_token(p);
			if (p->token_type != T_WORD &&
				p->token_type != T_SINGLE_QUOTED &&
				p->token_type != T_DOUBLE_QUOTED)
			{
				ft_putstr_fd("minishell: syntax error near unexpected token `", STDERR_FILENO);
				if (p->token_type == T_EOF)
					ft_putstr_fd("newline", STDERR_FILENO);
				else if (p->token_value)
					ft_putstr_fd(p->token_value, STDERR_FILENO);
				else
					ft_putstr_fd(" ", STDERR_FILENO);
				ft_putstr_fd("'\n", STDERR_FILENO);
				free(redir);
				ft_lstclear(&args, free_arg);
				free_redirections(redirs);
				return (NULL);
			}
			p->redir_fd = 0;
			redir->filename = ft_strdup(p->token_value);
			redir->next = NULL;
			*redir_tail = redir;
			redir_tail = &redir->next;
		}
		next_token(p);
	}
	cmd = malloc(sizeof(t_cmd));
	if (!cmd)
	{
		ft_lstclear(&args, free_arg);
		return (NULL);
	}
	cmd->args = build_expanded_args(args);
	cmd->redirections = redirs;
	cmd->next = NULL;
	ft_lstclear(&args, free_arg);
	return (cmd);
}

/*
** Checks if a token type is a redirection
** @param type Token type to check
** @return 1 if redirection, 0 otherwise
*/
int	is_redirection(t_token type)
{
	return (type == T_REDIR_IN || type == T_REDIR_OUT
		|| type == T_APPEND || type == T_HEREDOC);
}

/*
** Parses a pipeline of commands
** @param p Parser structure
** @return Linked list of command structures
*/
t_cmd	*parse_pipeline(t_parse *p)
{
	t_cmd	*head;
	t_cmd	**curr;
	t_cmd	*cmd;
	char	*error_token;

	head = NULL;
	curr = &head;
	next_token(p);
	if (p->token_type == T_PIPE || p->token_type == T_SEMICOLON)
	{
		syntax_error(p->token_value);
		return (NULL);
	}
	while (1)
	{
		cmd = parse_args(p);
		if (!cmd)
			break ;
		*curr = cmd;
		curr = &cmd->next;
		if (p->token_type != T_PIPE)
			break ;
		next_token(p);
		error_token = p->token_value;
		if (p->token_type == T_EOF)
			error_token = "newline";
		else if (p->token_type == T_PIPE || p->token_type == T_SEMICOLON)
			error_token = p->token_value;
		if (p->token_type == T_PIPE || p->token_type == T_SEMICOLON || p->token_type == T_EOF)
		{
			syntax_error(error_token);
			free_cmd(head);
			return (NULL);
		}
	}
	return (head);
}

/**
 * @brief Converts a list of t_arg structs to an array of expanded strings.
 *
 * @param args Linked list of t_arg structs.
 * @return char** Array of expanded arguments. Must be freed with ft_free_array.
 */
char	**build_expanded_args(t_list *args)
{
	char		**arr;
	t_list		*current;
	int			i;
	t_arg		*a;

	arr = malloc(sizeof(char *) * (ft_lstsize(args) + 1));
	current = args;
	i = 0;
	while (current)
	{
		a = (t_arg *)current->content;
		if (a->type == T_SINGLE_QUOTED)
			arr[i++] = ft_strdup(a->value);
		else
			arr[i++] = expand_variables(a->value);
		current = current->next;
	}
	arr[i] = NULL;
	return (arr);
}
