/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   handle_special.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jcologne <jcologne@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/23 14:06:27 by jcologne          #+#    #+#             */
/*   Updated: 2025/04/23 14:20:17 by jcologne         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static void	assign_value(t_parse *p, enum e_token t, char *v, int n)
{
	p->token_type = t;
	p->token_value = ft_strdup(v);
	if (n)
		next_char(p);
}

static void	handle_file_descriptor(t_parse *p)
{
	int		fd;
	char	*fd_str;

	fd = 0;
	if (ft_isdigit(p->curr_char))
	{
		fd_str = parse_fd(p);
		fd = ft_atoi(fd_str);
		free(fd_str);
		p->redir_fd = fd;
	}
}

static void	handle_output_redirection(t_parse *p)
{
	next_char(p);
	if (p->curr_char == '>')
		assign_value(p, T_APPEND, ">>", 1);
	else
		assign_value(p, T_REDIR_OUT, ">", 0);
}

static void	handle_input_redirection(t_parse *p)
{
	next_char(p);
	if (p->curr_char == '<')
		assign_value(p, T_HEREDOC, "<<", 1);
	else
		assign_value(p, T_REDIR_IN, "<", 0);
}

void	handle_special(t_parse *p)
{
	handle_file_descriptor(p);
	if (p->curr_char == '|')
		assign_value(p, T_PIPE, "|", 1);
	else if (p->curr_char == '>')
		handle_output_redirection(p);
	else if (p->curr_char == '<')
		handle_input_redirection(p);
}
