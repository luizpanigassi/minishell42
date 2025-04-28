/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_1.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: luinasci <luinasci@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/26 18:43:01 by luinasci          #+#    #+#             */
/*   Updated: 2025/04/28 19:46:05 by luinasci         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/**
 * @brief Advances parser past whitespace characters.
 * @param p Parser state.
 * @note Modifies parser position and current character.
 */
static void	skip_whitespace(t_parse *p)
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
static void	handle_quotes(t_parse *p, char quote)
{
	size_t	start;
	char	*content;

	start = p->pos;
	next_char(p);
	while (p->curr_char && (p->curr_char != quote
			|| (p->input[p->pos - 1] == '\\')))
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
		return ;
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
static void	handle_word(t_parse *p)
{
	size_t	start;
	char	*sub;

	start = p->pos;
	while (p->curr_char && !ft_isspace(p->curr_char)
		&& !is_special_char(p->curr_char))
	{
		next_char(p);
	}
	sub = ft_substr(p->input, start, p->pos - start);
	p->token_value = ft_strdup(sub);
	free(sub);
	p->token_type = T_WORD;
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
	else if (p->curr_char == '|' || p->curr_char == ';' || p->curr_char == '>'
		|| p->curr_char == '<')
		handle_special(p);
	else
		handle_word(p);
}
