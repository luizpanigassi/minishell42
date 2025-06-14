/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils_5.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: luinasci <luinasci@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/16 15:13:15 by jcologne          #+#    #+#             */
/*   Updated: 2025/05/07 19:36:57 by luinasci         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/**
 * @brief Extracts file descriptor number from input.
 * @param p Parser state.
 * @return String containing numeric file descriptor.
 * @note Advances parser position past digits.
 */
char	*parse_fd(t_parse *p)
{
	int	start;

	start = p->pos;
	while (ft_isdigit(p->curr_char))
		next_char(p);
	return (ft_substr(p->input, start, p->pos - start));
}

/**
 * @brief Updates the quote state based on the current character.
 * @param c The current character.
 * @param in_quote Pointer to the in_quote flag.
 * @param quote_char Pointer to the current quote character.
 */
void	update_quote_state(char c, int *in_quote, char *quote_char)
{
	if (*in_quote && c == *quote_char)
		*in_quote = 0;
	else if (!*in_quote && (c == '\'' || c == '"'))
	{
		*in_quote = 1;
		*quote_char = c;
	}
}

/**
 * @brief Processes the quote state for the current character.
 * @param str Pointer to the current position in the string.
 * @param in_quote Pointer to the in_quote flag.
 * @param quote_char Pointer to the current quote character.
 * @note Updates the quote state based on the character encountered.
 */
void	process_quote_state(const char *str, int *in_quote, char *quote_char)
{
	if (*str == '\'' || *str == '"')
		update_quote_state(*str, in_quote, quote_char);
}

/**
 * @brief Adds a substring to the result array.
 * @param result Pointer to the result array.
 * @param count Pointer to the current count of elements in the array.
 * @param start Pointer to the start of the substring.
 * @param end Pointer to the end of the substring.
 */
void	add_substring(char ***result, int *count,
	const char *start, const char *end)
{
	*result = realloc(*result, sizeof(char *) * (*count + 2));
	(*result)[(*count)++] = ft_substr(start, 0, end - start);
}

/**
 * @brief Handles a delimiter in the string, adding a
 *substring to the result array.
 * @param str Pointer to the current position in the string.
 * @param start Pointer to the start of the current substring.
 * @param result Pointer to the result array.
 * @param count Pointer to the current count of elements in the array.
 */
void	handle_delimiter(const char **str, const char **start,
	char ***result, int *count)
{
	if ((*str > *start) && (*str)[-1] == '\\')
		return ;
	add_substring(result, count, *start, *str);
	*start = *str + 1;
}
