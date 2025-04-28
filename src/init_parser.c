/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init_parser.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: luinasci <luinasci@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/17 09:53:29 by jcologne          #+#    #+#             */
/*   Updated: 2025/04/28 15:50:58 by luinasci         ###   ########.fr       */
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
