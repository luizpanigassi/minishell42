/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils_1.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: luinasci <luinasci@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/26 18:12:41 by luinasci          #+#    #+#             */
/*   Updated: 2025/04/28 19:47:32 by luinasci         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/**
 * @brief Frees null-terminated string array.
 * @param array Array to free.
 * @note Handles NULL input safely.
 */
void	ft_free_array(char **args)
{
	int	i;

	if (!args)
		return ;
	i = -1;
	while (args[++i])
		free(args[i]);
	free(args);
}

/**
 * @brief Checks if character is whitespace.
 * @param c Character to check.
 * @return 1 if whitespace, 0 otherwise.
 * @note Includes all standard whitespace characters.
 */
int	ft_isspace(int c)
{
	return (c == ' ' || c == '\t' || c == '\n' || c == '\r'
		|| c == '\f' || c == '\v');
}

/**
 * @brief Advances parser to next input character.
 * @param p Parser state.
 * @note Handles end-of-input detection.
 */
void	next_char(t_parse *p)
{
	if (p->input[p->pos])
	{
		p->pos++;
		p->curr_char = p->input[p->pos];
	}
	else
	{
		p->curr_char = '\0';
	}
}

/**
 * @brief Checks if character is shell metacharacter.
 * @param c Character to check.
 * @return 1 if special character, 0 otherwise.
 * @note Includes |, >, <, and ;.
 */
int	is_special_char(char c)
{
	return (c == '|' || c == '>' || c == '<');
}

/**
 * @brief Converts linked list to string array.
 * @param lst List of string elements.
 * @return Null-terminated string array.
 * @note Performs deep copy of list contents.
 */
char	**list_to_array(t_list *lst)
{
	char	**arr;
	int		count;
	int		i;
	t_list	*tmp;

	count = ft_lstsize(lst);
	arr = malloc((count + 1) * sizeof(char *));
	if (!arr)
		return (NULL);
	i = 0;
	tmp = lst;
	while (tmp)
	{
		arr[i] = ft_strdup((char *)tmp->content);
		if (!arr[i])
		{
			ft_free_array(arr);
			return (NULL);
		}
		tmp = tmp->next;
		i++;
	}
	arr[i] = NULL;
	return (arr);
}
