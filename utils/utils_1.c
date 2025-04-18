/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils_1.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: luinasci <luinasci@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/26 18:12:41 by luinasci          #+#    #+#             */
/*   Updated: 2025/04/18 14:40:55 by luinasci         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/*
** Frees an array of strings
** @param args Array to free
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

/*
** Checks if character is whitespace
** @param c Character to check
** @return 1 if whitespace, 0 otherwise
*/
int	ft_isspace(int c)
{
	return (c == ' ' || c == '\t' || c == '\n' || c == '\r'
		|| c == '\f' || c == '\v');
}

/*
** Advances to next character in parser input
** @param p Parser structure
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

/*
** Checks if character is special shell character
** @param c Character to check
** @return 1 if special, 0 otherwise
*/
int	is_special_char(char c)
{
	return (c == '|' || c == '>' || c == '<');
}

/*
** Converts a linked list to string array
** @param lst Linked list to convert
** @return New allocated string array
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
