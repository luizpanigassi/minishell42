/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: luinasci <luinasci@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/26 18:43:01 by luinasci          #+#    #+#             */
/*   Updated: 2025/04/11 18:05:24 by luinasci         ###   ########.fr       */
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
	p->redir_fd = 0;
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
	while (p->curr_char && (p->curr_char != quote || (p->input[p->pos - 1] == '\\')))
	{
		if (p->curr_char == '\\' && quote == '"')
			next_char(p); // Skip escape char if inside double quotes
		next_char(p);
	}

	if (!p->curr_char)
	{
		ft_putstr_fd("minishell: unmatched quote\n", STDERR_FILENO);
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
	while (p->curr_char && !ft_isspace(p->curr_char) && !is_special_char(p->curr_char))
	{
		next_char(p);
	}
	char *sub = ft_substr(p->input, start, p->pos - start);
	p->token_value = ft_strdup(sub);
	free(sub);
	p->token_type = T_WORD;
}

/*
** Handles special characters (|, >, <, etc.)
** @param p Parser structure
*/
static void handle_special(t_parse *p)
{
	int fd = 0;
	if (ft_isdigit(p->curr_char))
	{
		char *fd_str = parse_fd(p);
		fd = atoi(fd_str);
		free(fd_str);
	}
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
		p->redir_fd = fd;
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
		handle_word(p); // Ensures words don’t include unquoted special chars
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
	t_redir **redir_tail = &redirs;

	while (p->token_type != T_EOF && p->token_type != T_PIPE && p->token_type != T_SEMICOLON)
	{
		if (p->token_type == T_WORD || p->token_type == T_SINGLE_QUOTED || p->token_type == T_DOUBLE_QUOTED)
		{
			t_arg *arg = malloc(sizeof(t_arg));
			arg->value = ft_strdup(p->token_value);
			arg->type = p->token_type;
			ft_lstadd_back(&args, ft_lstnew(arg));
		}
		// Handle redirection operators
		else if (is_redirection(p->token_type))
		{
			t_redir *redir = malloc(sizeof(t_redir));
			redir->fd = p->redir_fd; // Capture the parsed FD
			if (!redir)
			{
				ft_lstclear(&args, free_arg);
				return (NULL);
			}

			redir->type = p->token_type;
			next_token(p); // Move to filename token

			// Check if filename is valid (word or quoted)
			if (p->token_type != T_WORD &&
				p->token_type != T_SINGLE_QUOTED &&
				p->token_type != T_DOUBLE_QUOTED)
			{
				// Syntax error: print message and clean up
				ft_putstr_fd("minishell: syntax error near unexpected token `", STDERR_FILENO);
				if (p->token_type == T_EOF)
					ft_putstr_fd("newline", STDERR_FILENO);
				else if (p->token_value)
					ft_putstr_fd(p->token_value, STDERR_FILENO);
				else
					ft_putstr_fd(" ", STDERR_FILENO); // Fallback
				ft_putstr_fd("'\n", STDERR_FILENO);

				free(redir);
				ft_lstclear(&args, free_arg);
				free_redirections(redirs); // Free existing redirections
				return (NULL);
			}
			p->redir_fd = 0; // Reset after use
			redir->filename = ft_strdup(p->token_value);
			redir->next = NULL;

			// Append to redirection list
			*redir_tail = redir;
			redir_tail = &redir->next;
		}
		next_token(p); // Advance to next token
	}

	// Build command structure
	t_cmd *cmd = malloc(sizeof(t_cmd));
	if (!cmd)
	{
		ft_lstclear(&args, free_arg);
		return (NULL);
	}

	// Convert argument list to expanded string array
	cmd->args = build_expanded_args(args);
	cmd->redirections = redirs; // Attach redirections
	cmd->next = NULL;			// Initialize pipeline pointer

	// Cleanup temporary structures
	ft_lstclear(&args, free_arg); // Free argument list and t_arg structs
	return (cmd);
}

/**
 * @brief Creates a temporary heredoc file.
 * @param delimiter Heredoc termination string.
 * @return File descriptor of the heredoc temp file.
 * @note Automatically deletes the temp file after opening.
 */
int create_heredoc(const char *delimiter)
{
	int pipefd[2];
	pid_t pid;
	int status;

	if (pipe(pipefd)) // Create a pipe
		return (-1);

	pid = fork();
	if (pid == -1)
	{
		close(pipefd[0]);
		close(pipefd[1]);
		return (-1);
	}

	if (pid == 0) // Child process
	{
		char *line;
		close(pipefd[0]); // Close unused read end

		// Reset signals to default/ignore for heredoc context
		signal(SIGINT, SIG_DFL);  // Allow Ctrl-C to terminate child
		signal(SIGQUIT, SIG_IGN); // Ignore Ctrl-slash
		int quoted_delimiter = (delimiter[0] == '\'' || delimiter[0] == '"');

		while (1)
		{
			line = readline("> ");
			if (!line)
				break;
			if (strcmp(line, delimiter) == 0)
			{
				free(line);
				break;
			}
			if (!quoted_delimiter)
			{ // NEW: Expand variables only if unquoted
				char *expanded = expand_variables(line);
				write(pipefd[1], expanded, strlen(expanded));
				free(expanded);
			}
			else
			{
				write(pipefd[1], line, strlen(line));
			}
			write(pipefd[1], "\n", 1);
			free(line);
		}
		close(pipefd[1]);
		exit(0); // Child exits normally
	}
	else
	{					  // Parent process
		close(pipefd[1]); // Close unused write end

		// Wait for child to finish
		waitpid(pid, &status, 0);

		if (WIFSIGNALED(status) && WTERMSIG(status) == SIGINT)
		{
			// Heredoc interrupted by Ctrl-C
			close(pipefd[0]);
			rl_replace_line("", 0);
			rl_on_new_line();
			ft_putstr_fd("\n", STDOUT_FILENO); // Mimic bash behavior
			set_exit_status(130);			   // Set $? = 130 (SIGINT exit code)
			return (-1);
		}

		return (pipefd[0]); // Return read end of pipe
	}
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

	// Check for leading pipe/semicolon
	if (p->token_type == T_PIPE || p->token_type == T_SEMICOLON)
	{
		syntax_error(p->token_value);
		return NULL;
	}

	while (1)
	{
		t_cmd *cmd = parse_args(p);
		if (!cmd)
			break;

		*curr = cmd;
		curr = &cmd->next;

		if (p->token_type != T_PIPE)
			break;

		next_token(p);

		// Handle pipe followed by invalid token
		char *error_token = p->token_value;
		if (p->token_type == T_EOF)
			error_token = "newline";
		else if (p->token_type == T_PIPE || p->token_type == T_SEMICOLON)
			error_token = p->token_value;

		if (p->token_type == T_PIPE || p->token_type == T_SEMICOLON || p->token_type == T_EOF)
		{
			syntax_error(error_token);
			free_cmd(head);
			return NULL;
		}
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
