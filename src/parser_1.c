/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_1.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: luinasci <luinasci@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/26 18:43:01 by luinasci          #+#             */
/*   Updated: 2025/05/07 14:42:01 by luinasci         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/**
 * @brief Advances the parser to the closing quote.
 * @param p Parser state.
 * @param quote Quote character (' or ").
 * @return 1 if the closing quote is found, 0 otherwise.
 */
int	advance_to_closing_quote(t_parse *p, char quote)
{
	while (p->curr_char && (p->curr_char != quote
			|| (p->input[p->pos - 1] == '\\' && quote == '"')))
	{
		if (p->curr_char == '\\' && quote == '"')
			next_char(p);
		next_char(p);
	}
	return (p->curr_char != '\0');
}

/**
 * @brief Adds a segment to the token value.
 * @param value Current token value.
 * @param input Input string.
 * @param start Start position of the segment.
 * @param end End position of the segment.
 * @return Updated token value.
 */
char	*add_segment(char *value, const char *input, size_t start, size_t end)
{
	char	*segment;
	char	*new_value;

	segment = ft_substr(input, start, end - start);
	new_value = ft_strjoin_free(value, segment);
	free(segment);
	return (new_value);
}

/**
 * @brief Handles a quoted segment within a word.
 * @param p Parser state.
 * @param value Current token value.
 * @param quote_state State of the quote handling.
 * @return Updated token value.
 */
char	*process_quoted_segment(t_parse *p,
		char *value, t_quote_state *quote_state)
{
	value = add_segment(value, p->input, *(quote_state->segment_start), p->pos);
	*(quote_state->in_quote) = !(*(quote_state->in_quote));
	*(quote_state->quote_char) = p->curr_char;
	next_char(p);
	*(quote_state->segment_start) = p->pos;
	return (value);
}

/**
 * @brief Main lexical analyzer - gets next token.
 * @param p Parser state.
 * @note Updates token type/value and parser position.
 */
void	next_token(t_parse *p)
{
	skip_whitespace(p);
	free(p->token_value);
	p->token_value = NULL;
	if (!p->curr_char)
	{
		p->token_type = T_EOF;
		return ;
	}
	if (p->curr_char == '\'' || p->curr_char == '"')
		handle_quotes(p, p->curr_char);
	else if (p->curr_char == '|' || p->curr_char == ';'
		|| p->curr_char == '>' || p->curr_char == '<')
		handle_special(p);
	else
		handle_word(p);
}

/**
 * @brief Processes unquoted word tokens.
 * @param p Parser state.
 * @note Collects characters until whitespace or special character.
 */
void	handle_word(t_parse *p)
{
	t_word_state	word_state;
	t_quote_state	quote_state;

	init_word_and_quote_states(p, &word_state, &quote_state);
	while (p->curr_char && (!ft_isspace(p->curr_char)
			&& (!is_special_char(p->curr_char) || word_state.in_quote)))
	{
		if (!word_state.in_quote && (p->curr_char == '\''
				|| p->curr_char == '"'))
			word_state.value = process_quoted_segment(p,
					word_state.value, &quote_state);
		else if (word_state.in_quote && p->curr_char == word_state.quote_char)
			word_state.value = handle_end_of_quote(p, word_state.value,
					&word_state.segment_start, &word_state.in_quote);
		else
			next_char(p);
	}
	word_state.value = add_segment(word_state.value,
			p->input, word_state.segment_start, p->pos);
	p->token_value = word_state.value;
	p->token_type = T_WORD;
}
