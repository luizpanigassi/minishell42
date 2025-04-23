/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_2.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jcologne <jcologne@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/17 09:46:38 by jcologne          #+#    #+#             */
/*   Updated: 2025/04/23 14:37:24 by jcologne         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

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
