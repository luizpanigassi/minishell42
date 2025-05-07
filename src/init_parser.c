/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init_parser.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: luinasci <luinasci@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/17 09:53:29 by jcologne          #+#    #+#             */
/*   Updated: 2025/05/07 16:08:24 by luinasci         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <minishell.h>

/**
 * @brief Initializes the parser state for new input.
 * @param p Parser structure to initialize.
 * @param input Input string to parse.
 */
void	init_parser(t_parse *p, char *input)
{
	p->input = input;
	p->pos = 0;
	p->curr_char = input[0];
	p->token_type = T_EOF;
	p->token_value = NULL;
	p->redir_fd = 0;
	p->syntax_error = 0;
}

/**
 * @brief Initializes the word and quote states for handle_word.
 * @param p Parser state.
 * @param word_state Pointer to the word state structure.
 * @param quote_state Pointer to the quote state structure.
 */
void	init_word_and_quote_states(t_parse *p,
	t_word_state *word_state, t_quote_state *quote_state)
{
word_state->segment_start = p->pos;
word_state->in_quote = 0;
word_state->quote_char = 0;
word_state->value = ft_strdup("");

quote_state->segment_start = &word_state->segment_start;
quote_state->in_quote = &word_state->in_quote;
quote_state->quote_char = &word_state->quote_char;
}
