/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init_parser.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jcologne <jcologne@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/17 09:53:29 by jcologne          #+#    #+#             */
/*   Updated: 2025/04/17 11:33:14 by jcologne         ###   ########.fr       */
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
}
