/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils_2.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jcologne <jcologne@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/16 14:40:41 by jcologne          #+#    #+#             */
/*   Updated: 2025/04/16 14:58:50 by jcologne         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/*
** Frees a command structure
** @param cmd Command to free
*/
void	free_cmd(t_cmd *cmd)
{
	t_redir	*redir;
	t_redir	*tmp;

	if (!cmd)
		return ;
	ft_free_array(cmd->args);
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

/*
** String comparison
** @param s1 First string
** @param s2 Second string
** @return Difference between first differing characters
*/
int	ft_strcmp(const char *s1, const char *s2)
{
	while (*s1 && *s1 == *s2)
	{
		s1++;
		s2++;
	}
	return (*(unsigned char *)s1 - *(unsigned char *)s2);
}

/*
** Checks if string is numeric
** @param str String to check
** @return 1 if numeric, 0 otherwise
*/
int	ft_isnumber(const char *str)
{
	int	i;

	if (!str || !*str)
		return (0);
	i = 0;
	if (str[i] == '+' || str[i] == '-')
		i++;
	while (str[i])
	{
		if (!ft_isdigit(str[i]))
			return (0);
		i++;
	}
	if (i > 0 && (str[0] == '+' || str[0] == '-'))
	{
		return (i > 1);
	}
	else
	{
		return (i > 0);
	}
}

/*
** Checks if string is valid variable name
** @param name String to check
** @return 1 if valid, 0 otherwise
*/
int	is_valid_var_name(const char *name)
{
	int	i;

	if (!name || !name[0] || ft_isdigit(name[0]))
		return (0);
	i = 0;
	while (name[i])
	{
		if (!ft_isalnum(name[i]) && name[i] != '_')
			return (0);
		i++;
	}
	return (1);
}

/*
** Prints environment variables in export format
*/
void	print_export_declarations(void)
{
	extern char	**environ;
	int			i;
	char		*eq;

	i = 0;
	while (environ[i])
	{
		eq = ft_strchr(environ[i], '=');
		if (eq)
		{
			*eq = '\0';
			printf("declare -x %s=\"%s\"\n", environ[i], eq + 1);
			*eq = '=';
		}
		else
			printf("declare -x %s\n", environ[i]);
		i++;
	}
}
