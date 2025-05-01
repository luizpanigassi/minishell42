/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_1.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: luinasci <luinasci@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/26 18:43:01 by luinasci          #+#    #+#             */
/*   Updated: 2025/04/30 18:49:01 by luinasci         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

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

/**
 * @brief Processes quoted string tokens.
 * @param p Parser state.
 * @param quote Quote character (' or ").
 * @note Handles escape characters and nested quotes.
 */
void handle_quotes(t_parse *p, char quote)
{
	size_t start;
	char *content;

	start = p->pos;
	next_char(p);
	while (p->curr_char && (p->curr_char != quote || (p->input[p->pos - 1] == '\\')))
	{
		if (p->curr_char == '\\' && quote == '"')
			next_char(p);
		next_char(p);
	}
	if (!p->curr_char)
	{
		ft_putstr_fd("minishell: unmatched quote\n", STDERR_FILENO);
		p->token_value = NULL;
		p->syntax_error = 1;
		return;
	}
	content = ft_substr(p->input, start + 1, p->pos - start - 1);
	if (quote == '\'')
		p->token_type = T_SINGLE_QUOTED;
	else
		p->token_type = T_DOUBLE_QUOTED;
	free(p->token_value);
	p->token_value = content;
	next_char(p);
}

/**
 * @brief Processes unquoted word tokens.
 * @param p Parser state.
 * @note Collects characters until whitespace or special character.
 */
void handle_word(t_parse *p)
{
	size_t start = p->pos;
	char *value = ft_strdup("");
	int in_quote = 0;
	char quote_char = 0;
	size_t segment_start = start;
	char *segment;

	while (p->curr_char && (!ft_isspace(p->curr_char) && (!is_special_char(p->curr_char) || in_quote)))
	{
		if (!in_quote && (p->curr_char == '\'' || p->curr_char == '"'))
		{
			// Add unquoted segment
			segment = ft_substr(p->input, segment_start, p->pos - segment_start);
			value = ft_strjoin_free(value, segment);
			free(segment);
			// Enter quoted state
			in_quote = 1;
			quote_char = p->curr_char;
			next_char(p);
			segment_start = p->pos;
		}
		else if (in_quote && p->curr_char == quote_char)
		{
			// Add quoted segment
			segment = ft_substr(p->input, segment_start, p->pos - segment_start);
			value = ft_strjoin_free(value, segment);
			free(segment);
			in_quote = 0;
			next_char(p);
			segment_start = p->pos;
		}
		else
			next_char(p);
	}
	// Add remaining segment
	segment = ft_substr(p->input, segment_start, p->pos - segment_start);
	value = ft_strjoin_free(value, segment);
	free(segment);

	p->token_value = value;
	p->token_type = T_WORD;
}

/**
 * @brief Main lexical analyzer - gets next token.
 * @param p Parser state.
 * @note Updates token type/value and parser position.
 */
void next_token(t_parse *p)
{
	skip_whitespace(p);
	free(p->token_value);
	p->token_value = NULL;
	if (!p->curr_char)
	{
		p->token_type = T_EOF;
		return;
	}
	if (p->curr_char == '\'' || p->curr_char == '"')
		handle_quotes(p, p->curr_char);
	else if (p->curr_char == '|' || p->curr_char == ';' || p->curr_char == '>' || p->curr_char == '<')
		handle_special(p);
	else
		handle_word(p);
}
