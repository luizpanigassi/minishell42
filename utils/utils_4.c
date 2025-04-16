/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils_4.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jcologne <jcologne@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/16 15:08:11 by jcologne          #+#    #+#             */
/*   Updated: 2025/04/16 15:11:01 by jcologne         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/**
 * @brief Safely appends a character to a string.
 * @param str Original string (freed after appending).
 * @param c Character to append.
 * @return New allocated string with appended character.
 */
char	*ft_strjoin_char(char *str, char c)
{
	char	*new_str;
	size_t	len;

	if (!str)
		len = 0;
	else
		len = ft_strlen(str);
	new_str = malloc(len + 2);
	if (!new_str)
		return (NULL);
	if (str)
		ft_memcpy(new_str, str, len);
	new_str[len] = c;
	new_str[len + 1] = '\0';
	free(str);
	return (new_str);
}

/**
 * @brief Counts the number of commands in a pipeline.
 *
 * @param pipeline Head of the command pipeline (linked list of t_cmd).
 * @return size_t Number of commands in the pipeline.
 * Returns 0 if pipeline is NULL.
 * @note Traverses the linked list of commands until reaching NULL.
 */
size_t	ft_cmd_size(t_cmd *pipeline)
{
	size_t	count;

	count = 0;
	while (pipeline)
	{
		count++;
		pipeline = pipeline->next;
	}
	return (count);
}

void	free_redirections(t_redir *redirs)
{
	t_redir	*tmp;

	while (redirs)
	{
		tmp = redirs->next;
		free(redirs->filename);
		free(redirs);
		redirs = tmp;
	}
}

/**
 * @brief Frees a copy of the environment variables.
 * @param env_copy The environment copy to free.
 */
void	free_env_copy(char **env_copy)
{
	if (!env_copy)
		return ;
	ft_free_array(env_copy);
}

void	syntax_error(char *token)
{
	ft_putstr_fd("minishell: syntax error near unexpected token '", STDERR_FILENO);
	if (!token || *token == '\0')
		ft_putstr_fd("newline", STDERR_FILENO);
	else
		ft_putstr_fd(token, STDERR_FILENO);
	ft_putstr_fd("'\n", STDERR_FILENO);
	set_exit_status(SYNTAX_ERROR);
}
