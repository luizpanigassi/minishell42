/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils_5.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jcologne <jcologne@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/16 15:13:15 by jcologne          #+#    #+#             */
/*   Updated: 2025/04/16 15:42:10 by jcologne         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

char	*parse_fd(t_parse *p)
{
	int	start;

	start = p->pos;
	while (ft_isdigit(p->curr_char))
		next_char(p);
	return (ft_substr(p->input, start, p->pos - start));
}

/**
 * @brief Splits a string by a delimiter, ignoring delimiters inside quotes.
 * @param str The input string to split.
 * @param delim The delimiter character.
 * @return A NULL-terminated array of strings.
 */
char	**split_with_quotes(const char *str, char delim)
{
	const char	*start;
	char		**result;
	int			count;
	int			in_quote;
	char		quote_char;

	result = NULL;
	count = 0;
	in_quote = 0;
	quote_char = '\0';
	start = str;
	while (*str)
	{
		if ((*str == '\'' || *str == '"') && (in_quote == 0 || *str == quote_char))
		{
			if (in_quote)
				in_quote = 0;
			else
			{
				in_quote = 1;
				quote_char = *str;
			}
		}
		else if (*str == delim && !in_quote)
		{
			result = realloc(result, sizeof(char *) * (count + 2));
			result[count++] = ft_substr(start, 0, str - start);
			start = str + 1;
		}
		str++;
	}
	if (start != str)
	{
		result = realloc(result, sizeof(char *) * (count + 2));
		result[count++] = ft_substr(start, 0, str - start);
	}
	if (result)
		result[count] = NULL;
	return (result);
}
