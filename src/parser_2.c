/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_2.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: luinasci <luinasci@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/17 09:46:38 by jcologne          #+#    #+#             */
/*   Updated: 2025/05/07 16:50:14 by luinasci         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/**
 * @brief Converts a list of t_arg structs to an array of expanded strings.
 *
 * @param args Linked list of t_arg structs.
 * @return char** Array of expanded arguments. Must be freed with ft_free_array.
 */
char	**build_expanded_args(t_list *args)
{
	char	**arr;
	t_list	*current;
	int		i;
	t_arg	*a;
	char	*processed;

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
			arr[i++] = expand_variables(processed);
			free(processed);
		}
		current = current->next;
	}
	arr[i] = NULL;
	return (arr);
}
