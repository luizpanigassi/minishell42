/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: luinasci <luinasci@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/26 18:43:01 by luinasci          #+#    #+#             */
/*   Updated: 2025/04/07 18:04:02 by luinasci         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/**
 * @brief Initializes the parser state for new input.
 * @param p Parser structure to initialize.
 * @param input Input string to parse.
 */
void init_parser(t_parse *p, char *input)
{
	p->input = input;
	p->pos = 0;
	p->curr_char = input[0]; // Initialize first character
	p->token_type = T_EOF;	 // Default to EOF
	p->token_value = NULL;
}

/*
** Skips whitespace characters in input
** @param p Parser structure
*/
static void skip_whitespace(t_parse *p)
{
	while (ft_isspace(p->curr_char))
		next_char(p);
}

/*
** Handles quoted strings in input
** @param p Parser structure
** @param quote Type of quote (' or ")
*/
static void handle_quotes(t_parse *p, char quote)
{
	size_t start = p->pos;
	char *content;

	next_char(p); // Skip opening quote
	while (p->curr_char && p->curr_char != quote)
		next_char(p);

	if (!p->curr_char)
	{
		ft_putstr_fd("minishell: unmatched quote\n", STDERR_FILENO);
		p->token_type = T_EOF;
		return;
	}

	// Extract content inside quotes (without the quotes themselves)
	content = ft_substr(p->input, start + 1, p->pos - start - 1);

	// Set token type based on quote type
	if (quote == '\'')
		p->token_type = T_SINGLE_QUOTED;
	else
		p->token_type = T_DOUBLE_QUOTED;

	free(p->token_value); // Free previous value
	p->token_value = content;
	next_char(p); // Skip closing quote
}

/*
** Handles regular words in input
** @param p Parser structure
*/
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

/*
** Handles special characters (|, >, <, etc.)
** @param p Parser structure
*/
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
	else if (p->curr_char == '<')
	{
		next_char(p);
		if (p->curr_char == '<')
		{
			p->token_type = T_HEREDOC;
			p->token_value = ft_strdup("<<");
			next_char(p);
		}
		else
		{
			p->token_type = T_REDIR_IN;
			p->token_value = ft_strdup("<");
		}
	}
}

/*
** Gets the next token from input
** @param p Parser structure
*/
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

/*
** Parses command arguments and redirections
** @param p Parser structure
** @return Command structure with arguments and redirections
*/
t_cmd *parse_args(t_parse *p)
{
	t_list *args = NULL;
	t_redir *redirs = NULL;
	t_arg *arg;

	while (p->token_type != T_EOF && p->token_type != T_PIPE)
	{
		if (p->token_type == T_WORD ||
			p->token_type == T_SINGLE_QUOTED ||
			p->token_type == T_DOUBLE_QUOTED)
		{
			arg = malloc(sizeof(t_arg));
			arg->value = ft_strdup(p->token_value);
			arg->type = p->token_type;
			ft_lstadd_back(&args, ft_lstnew(arg));
		}
		else if (is_redirection(p->token_type))
		{
			// Redirection handling (unchanged)
		}
		next_token(p);
	}

	// Convert to command arguments with expansion
	t_cmd *cmd = malloc(sizeof(t_cmd));
	cmd->args = build_expanded_args(args); // New function
	cmd->redirections = redirs;
	cmd->next = NULL;

	// Cleanup temporary structures
	ft_lstclear(&args, free_arg);
	return cmd;
}

/**
 * @brief Creates a temporary heredoc file.
 * @param delimiter Heredoc termination string.
 * @return File descriptor of the heredoc temp file.
 * @note Automatically deletes the temp file after opening.
 */
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

/*
** Checks if a token type is a redirection
** @param type Token type to check
** @return 1 if redirection, 0 otherwise
*/
int is_redirection(t_token type)
{
	return (type == T_REDIR_IN || type == T_REDIR_OUT ||
			type == T_APPEND || type == T_HEREDOC);
}

/*
** Parses a pipeline of commands
** @param p Parser structure
** @return Linked list of command structures
*/
t_cmd *parse_pipeline(t_parse *p)
{
	t_cmd *head = NULL;
	t_cmd **curr = &head;

	next_token(p);

	while (1)
	{
		t_cmd *cmd = parse_args(p);
		if (!cmd)
			break;

		*curr = cmd;
		curr = &cmd->next;

		if (p->token_type != T_PIPE)
			break;

		next_token(p); // Consume the pipe
	}

	return head;
}

/**
 * @brief Converts a list of t_arg structs to an array of expanded strings.
 *
 * @param args Linked list of t_arg structs.
 * @return char** Array of expanded arguments. Must be freed with ft_free_array.
 */
char **build_expanded_args(t_list *args)
{
	int count = ft_lstsize(args);
	char **arr = malloc(sizeof(char *) * (count + 1));
	t_list *current = args;
	int i = 0;

	while (current)
	{
		t_arg *a = (t_arg *)current->content;
		char *expanded;

		if (a->type == T_SINGLE_QUOTED)
			expanded = ft_strdup(a->value); // No expansion
		else
			expanded = expand_variables(a->value); // Expand variables

		arr[i++] = expanded;
		current = current->next;
	}
	arr[i] = NULL;
	return arr;
}
