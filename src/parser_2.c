/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_2.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: luinasci <luinasci@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/17 09:46:38 by jcologne          #+#    #+#             */
/*   Updated: 2025/04/29 16:13:16 by luinasci         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/**
 * @brief Checks if token is redirection operator.
 * @param type Token type to check.
 * @return 1 if redirection token, 0 otherwise.
 * @note Covers all redirection types (>, >>, <, <<).
 */
int	is_redirection(t_token type)
{
	return (type == T_REDIR_IN || type == T_REDIR_OUT || type == T_APPEND
		|| type == T_HEREDOC);
}

/**
 * @brief Converts a list of t_arg structs to an array of expanded strings.
 *
 * @param args Linked list of t_arg structs.
 * @return char** Array of expanded arguments. Must be freed with ft_free_array.
 */
char	**build_expanded_args(t_list *args)
{
	char **arr;
	t_list *current;
	int i;
	t_arg *a;
	char *processed;

	arr = malloc(sizeof(char *) * (ft_lstsize(args) + 1));
	current = args;
	i = 0;
	while (current)
	{
		a = (t_arg *)current->content;
		if (a->type == T_SINGLE_QUOTED)
			arr[i++] = ft_strdup(a->value);
		else
		{
			processed = process_escapes(a->value);
			char *expanded = expand_variables(processed);
			arr[i++] = expanded;
			free(processed);
		}
		current = current->next;
	}
	arr[i] = NULL;
	return (arr);
}
