/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: luinasci <luinasci@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/26 18:12:41 by luinasci          #+#    #+#             */
/*   Updated: 2025/03/26 18:58:09 by luinasci         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void ft_free_array(char **args)
{
	int i;

	if (!args)
		return;
	i = -1;
	while (args[++i])
		free(args[i]);
	free(args);
}

int ft_isspace(int c)
{
	return (c == ' ' || c == '\t' || c == '\n' || c == '\r' || c == '\f' || c == '\v');
}

void next_char(t_parse *p)
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

int is_special_char(char c)
{
	return (c == '|' || c == '>' || c == '<' || c == ';');
}

char **list_to_array(t_list *lst)
{
	char **arr;
	int count;
	int i;
	t_list *tmp;

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
