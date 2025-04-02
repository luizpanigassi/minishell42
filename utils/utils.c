/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: luinasci <luinasci@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/26 18:12:41 by luinasci          #+#    #+#             */
/*   Updated: 2025/04/02 16:25:33 by luinasci         ###   ########.fr       */
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

void free_cmd(t_cmd *cmd)
{
	t_redir *redir;
	t_redir *tmp;

	if (!cmd)
		return;

	// Free arguments
	ft_free_array(cmd->args);

	// Free redirections
	redir = cmd->redirections;
	while (redir)
	{
		tmp = redir->next;
		free(redir->filename);
		free(redir);
		redir = tmp;
	}
	free(cmd);
}

int ft_strcmp(const char *s1, const char *s2)
{
	while (*s1 && *s1 == *s2)
	{
		s1++;
		s2++;
	}
	return (*(unsigned char *)s1 - *(unsigned char *)s2);
}

// In utils.c
char **ft_array_append(char **array, char *new_element)
{
	int count;
	char **new_array;

	count = 0;
	while (array && array[count])
		count++;

	new_array = malloc(sizeof(char *) * (count + 2));
	if (!new_array)
		return (NULL);

	int i = -1;
	while (++i < count)
		new_array[i] = array[i];

	new_array[count] = new_element;
	new_array[count + 1] = NULL;

	free(array);
	return (new_array);
}

int ft_isnumber(const char *str)
{
	int i;

	if (!str || !*str) // Handle NULL or empty string
		return (0);
	i = 0;
	// Allow optional leading +/-
	if (str[i] == '+' || str[i] == '-')
		i++;
	// Check remaining characters are digits
	while (str[i])
	{
		if (!ft_isdigit(str[i]))
			return (0);
		i++;
	}
	// Must have at least one digit (after optional sign)
	return (i > 0 && (str[0] == '+' || str[0] == '-') ? i > 1 : i > 0);
}
