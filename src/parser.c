/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: luinasci <luinasci@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/26 18:43:01 by luinasci          #+#    #+#             */
/*   Updated: 2025/03/28 16:17:19 by luinasci         ###   ########.fr       */
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
	char *sub;

	while (p->curr_char && !ft_isspace(p->curr_char) &&
		   !is_special_char(p->curr_char))
	{
		next_char(p);
	}
	sub = ft_substr(p->input, start, p->pos - start);
	p->token_value = ft_strdup(sub);
	free(sub); // Free the temporary substring
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

t_cmd *parse_args(t_parse *p)
{
	t_list *args = NULL;
	t_redir *redirs = NULL;

	next_token(p);
	while (p->token_type != T_EOF)
	{
		if (p->token_type == T_WORD)
		{
			ft_lstadd_back(&args, ft_lstnew(ft_strdup(p->token_value)));
		}
		else if (is_redirection(p->token_type))
		{ // Check if token is a redirection
			t_redir *redir = malloc(sizeof(t_redir));
			redir->type = p->token_type;
			next_token(p); // Move to filename token
			redir->filename = ft_strdup(p->token_value);
			redir->next = redirs;
			redirs = redir;
		}
		else if (p->token_type == T_HEREDOC)
		{
			t_redir *redir = malloc(sizeof(t_redir));
			redir->type = T_HEREDOC;
			next_token(p);
			redir->filename = ft_strdup(p->token_value); // Delimiter
			redir->next = redirs;
			redirs = redir;
		}
		next_token(p);
	}

	t_cmd *cmd = malloc(sizeof(t_cmd));
	cmd->args = list_to_array(args);
	cmd->redirections = redirs;
	ft_lstclear(&args, free);
	return cmd;
}

int create_heredoc(const char *delimiter)
{
	char *line;
	int tmp_fd = open(".heredoc.tmp", O_WRONLY | O_CREAT | O_TRUNC, 0644);

	while (1)
	{
		line = readline("> ");
		if (!line || ft_strcmp(line, delimiter) == 0)
			break;
		write(tmp_fd, line, ft_strlen(line));
		write(tmp_fd, "\n", 1);
		free(line);
	}
	close(tmp_fd);
	tmp_fd = open(".heredoc.tmp", O_RDONLY);
	unlink(".heredoc.tmp"); // Delete temp file after use
	return tmp_fd;
}

int is_redirection(t_token type)
{
	return (type == T_REDIR_IN || type == T_REDIR_OUT ||
			type == T_APPEND || type == T_HEREDOC);
}
