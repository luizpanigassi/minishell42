/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   handle_parse.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: luinasci <luinasci@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/07 15:59:37 by luinasci          #+#    #+#             */
/*   Updated: 2025/05/07 16:26:03 by luinasci         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/**
 * @brief Handles unmatched quotes by setting error state.
 * @param p Parser state.
 */
void	handle_unmatched_quote(t_parse *p)
{
	ft_putstr_fd("minishell: unmatched quote\n", STDERR_FILENO);
	p->token_value = NULL;
	p->syntax_error = 1;
}

/**
 * @brief Processes quoted string tokens.
 * @param p Parser state.
 * @param quote Quote character (' or ").
 * @note Handles escape characters and nested quotes.
 */
void	handle_quotes(t_parse *p, char quote)
{
	size_t	start;
	char	*content;
	char	*expanded;

	start = p->pos + 1;
	next_char(p);
	if (!advance_to_closing_quote(p, quote))
	{
		handle_unmatched_quote(p);
		return ;
	}
	content = ft_substr(p->input, start, p->pos - start);
	if (quote == '"')
	{
		expanded = expand_variables(content);
		free(content);
		content = expanded;
		p->token_type = T_DOUBLE_QUOTED;
	}
	else
		p->token_type = T_SINGLE_QUOTED;
	free(p->token_value);
	p->token_value = content;
	next_char(p);
}

/**
 * @brief Handles quoted segments in the input.
 * @param p Parser state.
 * @param value Current token value.
 * @param quote_state State of the quote handling.
 * @return Updated token value.
 */
char	*handle_quoted_segment(t_parse *p,
	char *value, t_quote_state *quote_state)
{
	value = add_segment(value, p->input, *(quote_state->segment_start), p->pos);
	*(quote_state->in_quote) = !(*(quote_state->in_quote));
	if (*(quote_state->in_quote))
		*(quote_state->quote_char) = p->curr_char;
	next_char(p);
	*(quote_state->segment_start) = p->pos;
	return (value);
}

/**
 * @brief Handles the end of a quoted segment within a word.
 * @param p Parser state.
 * @param value Current token value.
 * @param segment_start Pointer to the start position of the segment.
 * @param in_quote Pointer to the in_quote flag.
 * @return Updated token value.
 */
char	*handle_end_of_quote(t_parse *p, char *value,
	size_t *segment_start, int *in_quote)
{
	value = add_segment(value, p->input, *segment_start, p->pos);
	*in_quote = 0;
	next_char(p);
	*segment_start = p->pos;
	return (value);
}

/**
 * @brief Handles unquoted segments in the input.
 * @param p Parser state.
 * @param value Current token value.
 * @param segment_start Start position of the segment.
 * @return Updated token value.
 */
char	*handle_unquoted_segment(t_parse *p, char *value, size_t *segment_start)
{
	value = add_segment(value, p->input, *segment_start, p->pos);
	next_char(p);
	*segment_start = p->pos;
	return (value);
}
