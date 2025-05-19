/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils_6.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: luinasci <luinasci@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/07 16:30:01 by luinasci          #+#    #+#             */
/*   Updated: 2025/05/07 19:47:53 by luinasci         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/**
 * @brief Processes escape sequences in a string.
 * @param str Input string containing escape sequences.
 * @return A new string with escape sequences resolved.
 * @note Allocates memory for the result, which must be freed by the caller.
 */
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
void	skip_whitespace(t_parse *p)
{
	while (ft_isspace(p->curr_char))
		next_char(p);
}

/**
 * @brief Skips escaped characters in the string.
 * @param str Pointer to the current position in the string.
 * @param delim Delimiter character.
 * @return Updated pointer to the string after skipping.
 * @note Handles escaped delimiters and backslashes.
 */
const char	*skip_escaped(const char *str, char delim)
{
	if (*str == '\\' && (str[1] == delim || str[1] == '\\'))
		return (str + 2);
	return (str);
}

/**
 * @brief Finalizes the split by adding the last substring if necessary.
 * @param start Pointer to the start of the current substring.
 * @param str Pointer to the end of the string.
 * @param result Pointer to the result array.
 * @param count Pointer to the current count of elements in the array.
 * @note Ensures the last segment is added to the result array.
 */
void	finalize_split(const char *start, const char *str,
	char ***result, int *count)
{
	if (start != str)
		add_substring(result, count, start, str);
}
