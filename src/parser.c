/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: luinasci <luinasci@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/26 18:43:01 by luinasci          #+#    #+#             */
/*   Updated: 2025/03/27 18:55:47 by luinasci         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void init_parser(t_parse *p, char *input)
{
	p->input = input;
	p->pos = 0;
	p->curr_char = input[0]; // Initialize first character
	p->token_type = T_EOF;	 // Default to EOF
	p->token_value = NULL;
}

static void skip_whitespace(t_parse *p)
{
	while (ft_isspace(p->curr_char))
		next_char(p);
}

static void handle_quotes(t_parse *p, char quote)
{
	size_t start = p->pos;

	next_char(p); // Skip opening quote
	while (p->curr_char && p->curr_char != quote)
		next_char(p);

	if (!p->curr_char)
	{
		ft_putstr_fd("minishell: unmatched quote\n", 2);
		return;
	}

	p->token_value = ft_substr(p->input, start, p->pos - start + 1);
	p->token_type = T_WORD;
	next_char(p); // Skip closing quote
}

static void handle_word(t_parse *p)
{
	size_t start = p->pos;

	while (p->curr_char && !ft_isspace(p->curr_char) &&
		   !is_special_char(p->curr_char))
	{
		next_char(p);
	}

	// Extract substring and duplicate it
	p->token_value = ft_strdup(ft_substr(p->input, start, p->pos - start));
	p->token_type = T_WORD;
}

static void handle_special(t_parse *p)
{
	if (p->curr_char == '|')
	{
		p->token_type = T_PIPE;
		p->token_value = ft_strdup("|");
		next_char(p);
	}
	else if (p->curr_char == '>')
	{
		next_char(p);
		if (p->curr_char == '>')
		{
			p->token_type = T_APPEND;
			p->token_value = ft_strdup(">>");
			next_char(p);
		}
		else
		{
			p->token_type = T_REDIR_OUT;
			p->token_value = ft_strdup(">");
		}
	}
	// Add similar handling for < and <<
}

void next_token(t_parse *p)
{
	skip_whitespace(p);

	if (!p->curr_char)
	{
		p->token_type = T_EOF;
		return;
	}

	if (p->curr_char == '\'' || p->curr_char == '"')
		handle_quotes(p, p->curr_char);
	else if (is_special_char(p->curr_char))
		handle_special(p);
	else
		handle_word(p);
}

char **parse_args(t_parse *p)
{
	t_list *args = NULL;

	// PROCESS FIRST TOKEN
	next_token(p);

	while (p->token_type != T_EOF)
	{
		if (p->token_type == T_WORD)
		{
			char *token_copy = ft_strdup(p->token_value);
			ft_lstadd_back(&args, ft_lstnew(token_copy));
		}
		// Move to next token
		next_token(p);
	}

	char **result = list_to_array(args);
	ft_lstclear(&args, free);
	return result;
}
