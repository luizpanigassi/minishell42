/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils_5.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: luinasci <luinasci@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/16 15:13:15 by jcologne          #+#    #+#             */
/*   Updated: 2025/04/28 19:49:45 by luinasci         ###   ########.fr       */
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
	add_substring(result, count, *start, *str);
	*start = *str + 1;
}

/**
 * @brief Splits string while respecting quoted sections.
 * @param str Input string to split.
 * @param delim Delimiter character.
 * @return Array of split tokens.
 * @note Preserves quoted content including delimiters.
 */
char	**split_with_quotes(const char *str, char delim)
{
	const char	*start;
	char		**result;
	int			count;
	int			in_quote;
	char		quote_char;

	start = str;
	result = NULL;
	count = 0;
	in_quote = 0;
	quote_char = '\0';
	while (*str)
	{
		if ((*str == '\'' || *str == '"'))
			update_quote_state(*str, &in_quote, &quote_char);
		else if (*str == delim && !in_quote)
			handle_delimiter(&str, &start, &result, &count);
		str++;
	}
	if (start != str)
		add_substring(&result, &count, start, str);
	if (result)
		result[count] = NULL;
	return (result);
}
