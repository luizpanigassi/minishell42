/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils_7.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: luinasci <luinasci@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/07 16:47:09 by luinasci          #+#    #+#             */
/*   Updated: 2025/05/07 16:55:32 by luinasci         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

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
		str = skip_escaped(str, delim);
		if (!*str)
			break ;
		process_quote_state(str, &in_quote, &quote_char);
		if (*str == delim && !in_quote)
			handle_delimiter(&str, &start, &result, &count);
		str++;
	}
	finalize_split(start, str, &result, &count);
	if (result)
		result[count] = NULL;
	return (result);
}

/**
 * @brief Global error handler with exit.
 * @param message Error description to display.
 * @note Prints to stderr and exits with failure status.
 */
void	handle_error(char *message)
{
	perror(message);
	exit(EXIT_FAILURE);
}
