/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils_6.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: luinasci <luinasci@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/07 16:30:01 by luinasci          #+#    #+#             */
/*   Updated: 2025/05/07 16:30:26 by luinasci         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

char	*process_escapes(char *str)
{
	char	*result;
	int		i;
	int		j;

	result = malloc(ft_strlen(str) + 1);
	j = 0;
	i = 0;
	while (str[i])
	{
		if (str[i] == '\\')
		{
			i++;
			if (str[i] != '\0')
			{
				result[j++] = str[i++];
			}
		}
		else
		{
			result[j++] = str[i++];
		}
	}
	result[j] = '\0';
	return (result);
}

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
 * @brief Advances parser past whitespace characters.
 * @param p Parser state.
 * @note Modifies parser position and current character.
 */
void skip_whitespace(t_parse *p)
{
	while (ft_isspace(p->curr_char))
		next_char(p);
}
