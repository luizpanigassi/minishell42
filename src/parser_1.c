/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_1.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jcologne <jcologne@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/26 18:43:01 by luinasci          #+#    #+#             */
/*   Updated: 2025/04/17 09:55:16 by jcologne         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/*
** Skips whitespace characters in input
** @param p Parser structure
*/
static void	skip_whitespace(t_parse *p)
{
	while (ft_isspace(p->curr_char))
		next_char(p);
}

/*
** Handles quoted strings in input
** @param p Parser structure
** @param quote Type of quote (' or ")
*/
static void	handle_quotes(t_parse *p, char quote)
{
	size_t	start;
	char		*content;

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

/*
** Handles regular words in input
** @param p Parser structure
*/
static void	handle_word(t_parse *p)
{
	size_t	start;
	char		*sub;

	start = p->pos;
	while (p->curr_char && !ft_isspace(p->curr_char) && !is_special_char(p->curr_char))
	{
		next_char(p);
	}
	sub = ft_substr(p->input, start, p->pos - start);
	p->token_value = ft_strdup(sub);
	free(sub);
	p->token_type = T_WORD;
}

/*
** Handles special characters (|, >, <, etc.)
** @param p Parser structure
*/
static void	assign_value(t_parse *p, enum e_token t, char *v, int n)
{
	p->token_type = t;
	p->token_value = ft_strdup(v);
	if (n)
		next_char(p);
}

static void	handle_special(t_parse *p)
{
	int	fd;
	char	*fd_str;

	fd = 0;
	if (ft_isdigit(p->curr_char))
	{
		fd_str = parse_fd(p);
		fd = ft_atoi(fd_str);
		free(fd_str);
	}
	if (p->curr_char == '|')
		assign_value(p, T_PIPE, "|", 1);
	else if (p->curr_char == '>')
	{
		next_char(p);
		if (p->curr_char == '>')
			assign_value(p, T_APPEND, ">>", 1);
		else
			assign_value(p, T_REDIR_OUT, ">", 0);
		p->redir_fd = fd;
	}
	else if (p->curr_char == '<')
	{
		next_char(p);
		if (p->curr_char == '<')
			assign_value(p, T_HEREDOC, "<<", 1);
		else
			assign_value(p, T_REDIR_IN, "<", 0);
	}
}

/*
** Gets the next token from input
** @param p Parser structure
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
	else if (p->curr_char == '|' || p->curr_char == ';' || p->curr_char == '>' || p->curr_char == '<')
		handle_special(p);
	else
		handle_word(p);
}
