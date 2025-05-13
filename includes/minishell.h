/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: luinasci <luinasci@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/03 18:15:03 by luinasci          #+#    #+#             */
/*   Updated: 2025/05/13 15:24:36 by luinasci         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MINISHELL_H
# define MINISHELL_H

# include "../libft/libft.h"

# include <stdio.h>
# include <stdlib.h>
# include <unistd.h>
# include <string.h>
# include <sys/types.h>
# include <sys/wait.h>
# include <sys/stat.h>
# include <errno.h>
# include <signal.h>
# include <readline/readline.h>
# include <readline/history.h>
# include <dirent.h>
# include <limits.h>

# define CMD_NOT_FOUND 127
# define PERM_DENIED 126
# define SYNTAX_ERROR 2
# define EXIT_CODE_EXIT 4242

/**
 * @enum e_token
 * @brief Represents the different types of tokens used in the shell parser.
 *
 * @note
 * - `T_WORD`: A word token.
 * - `T_SINGLE_QUOTED`: A single-quoted string token.
 * - `T_DOUBLE_QUOTED`: A double-quoted string token.
 * - `T_PIPE`: A pipe ('|') token.
 * - `T_REDIR_OUT`: A redirection output ('>') token.
 * - `T_REDIR_IN`: A redirection input ('<') token.
 * - `T_APPEND`: An append ('>>') token.
 * - `T_HEREDOC`: A heredoc ('<<') token.
 * - `T_EOF`: End-of-file token.
 * - `T_SEMICOLON`: A semicolon (';') token.
 */
typedef enum e_token
{
	T_WORD,
	T_SINGLE_QUOTED,
	T_DOUBLE_QUOTED,
	T_PIPE,
	T_REDIR_OUT,
	T_REDIR_IN,
	T_APPEND,
	T_HEREDOC,
	T_EOF,
	T_SEMICOLON
}	t_token;

/**
 * @struct s_parse
 * @brief Represents the state of the parser during input parsing.
 *
 * @note
 * - `input`: The input string being parsed.
 * - `pos`: Current position in the input string.
 * - `curr_char`: Current character being processed.
 * - `token_type`: Type of the current token.
 * - `token_value`: Value of the current token.
 * - `redir_fd`: File descriptor for redirection.
 * - `syntax_error`: Flag indicating a syntax error.
 */
typedef struct s_parse
{
	char	*input;
	int		pos;
	char	curr_char;
	t_token	token_type;
	char	*token_value;
	int		redir_fd;
	int		syntax_error;
}	t_parse;

/**
 * @struct s_redir
 * @brief Represents a redirection in a command.
 *
 * @note
 * - `type`: Type of redirection (e.g., T_REDIR_OUT).
 * - `filename`: Filename for the redirection.
 * - `next`: Pointer to the next redirection.
 * - `heredoc_quoted`: Indicates if the heredoc is quoted.
 * - `fd`: File descriptor for the redirection.
 */
typedef struct s_redir {
	t_token			type;
	char			*filename;
	struct s_redir	*next;
	int				heredoc_quoted;
	int				fd;
}	t_redir;

/**
 * @struct s_cmd
 * @brief Represents a command in the shell.
 *
 * @note
 * - `args`: Arguments for the command.
 * - `redirections`: Linked list of redirections.
 * - `next`: Pointer to the next command in the pipeline.
 */
typedef struct s_cmd {
	char			**args;
	t_redir			*redirections;
	struct s_cmd	*next;
}	t_cmd;

/**
 * @struct s_arg
 * @brief Represents an argument with its type.
 *
 * @note
 * - `value`: The value of the argument.
 * - `type`: The type of the argument.
 */
typedef struct s_arg {
	char	*value;
	t_token	type;
}	t_arg;

/**
 * @struct s_export_params
 * @brief Represents parameters for the export command.
 *
 * @note
 * - `ret`: Pointer to the return value.
 * - `i`: Pointer to the current index.
 * - `arg`: The argument being processed.
 */
typedef struct s_export_params {
	int		*ret;
	int		*i;
	char	*arg;
}	t_export_params;

/**
 * @struct s_exec_vars
 * @brief Represents variables used during command execution.
 *
 * @note
 * - `prev_pipe`: File descriptors for the previous pipe.
 * - `next_pipe`: File descriptors for the next pipe.
 * - `child_pids`: Array of child process IDs.
 * - `pid`: Process ID of the current process.
 * - `current`: Pointer to the current command.
 * - `last_status`: Exit status of the last executed command.
 */
typedef struct s_exec_vars {
	int		prev_pipe[2];
	int		next_pipe[2];
	pid_t	*child_pids;
	pid_t	pid;
	t_cmd	*current;
	int		last_status;
}	t_exec_vars;

/**
 * @struct s_token_state
 * @brief Represents the state of tokens during parsing.
 *
 * @note
 * - `args`: List of arguments.
 * - `redirs`: List of redirections.
 * - `redir_tail`: Pointer to the tail of the redirection list.
 * - `has_args`: Flag indicating if arguments are present.
 */
typedef struct s_token_state
{
	t_list		**args;
	t_redir		**redirs;
	t_redir		***redir_tail;
	int			*has_args;
}	t_token_state;

/**
 * @struct s_pipeline_context
 * @brief Represents the context of a pipeline during execution.
 *
 * @note
 * - `prev_pipe`: File descriptors for the previous pipe.
 * - `next_pipe`: File descriptors for the next pipe.
 * - `child_pids`: Array of child process IDs.
 * - `current`: Pointer to the current command in the pipeline.
 * - `last_status`: Exit status of the last executed command.
 * - `index`: Index of the current command in the pipeline.
 */
typedef struct s_pipeline_context
{
	int		prev_pipe[2];
	int		next_pipe[2];
	pid_t	*child_pids;
	t_cmd	*current;
	int		last_status;
	int		index;
}	t_pipeline_context;

/**
 * @struct s_quote_state
 * @brief Represents the state of quotes during parsing.
 *
 * @note
 * - `segment_start`: Pointer to the start of the current segment.
 * - `in_quote`: Flag indicating if inside a quote.
 * - `quote_char`: Pointer to the current quote character.
 */
typedef struct s_quote_state
{
	size_t	*segment_start;
	int		*in_quote;
	char	*quote_char;
}	t_quote_state;

/**
 * @struct s_word_state
 * @brief Represents the state of a word during parsing.
 *
 * @note
 * - `segment_start`: Start position of the current segment.
 * - `in_quote`: Flag indicating if inside a quote.
 * - `quote_char`: Current quote character.
 * - `value`: Value of the word being parsed.
 */
typedef struct s_word_state
{
	size_t	segment_start;
	int		in_quote;
	char	quote_char;
	char	*value;
}	t_word_state;

typedef struct s_heredoc_context
{
	int					pipefd[2];
	struct sigaction	sa;
	struct sigaction	old_sa;
	int					was_signaled;
}	t_heredoc_context;

/*
** Global variable to store the exit status of commands
** volatile sig_atomic_t ensures safe access in signal handlers
*/
extern volatile sig_atomic_t	g_exit_status;

/**
 * @file builtin.c
 * @brief Contains functions related to built-in shell commands.
 */
// BUILTIN
int			is_builtin(char **tokens);
int			exec_builtin(char **args);
char		**ft_copy_env(char **original);
void		update_env_var(char *var, char *value);
void		ensure_var_exported(char *var_name);

/**
 * @file exec_cd.c
 * @brief Contains functions for handling the `cd` built-in command.
 */
// EXEC CD
int			exec_cd(char **args);
char		*resolve_cd_path(char **args, char *oldpwd);
int			handle_cd_arguments(char **args, char *oldpwd);

/**
 * @file exec_echo.c
 * @brief Contains functions for handling the `echo` built-in command.
 */
// EXEC ECHO
int			exec_echo(char **args);

/**
 * @file exec_env.c
 * @brief Contains functions for handling the `env` built-in command.
 */
// EXEC ENV
int			exec_env(char **args);

/**
 * @file exec_exit.c
 * @brief Contains functions for handling the `exit` built-in command.
 */
// EXEC EXIT
int			exec_exit(char **args);

/**
 * @file exec_export.c
 * @brief Contains functions for handling the `export` built-in command.
 */
// EXEC EXPORT
int			exec_export(char **args);
int			handle_invalid_identifier(char *var_name, char *value,
				t_export_params params);
int			handle_export_argument(char *arg, int *i, int *ret);

/**
 * @file exec_pwd.c
 * @brief Contains functions for handling the `pwd` built-in command.
 */
// EXEC PWD
int			exec_pwd(char **args);

/**
 * @file exec_unset.c
 * @brief Contains functions for handling the `unset` built-in command.
 */
// EXEC UNSET
int			handle_unset_error(char *arg);
void		shift_environment(char **env_ptr);
char		**find_and_free_env_var(const char *var_name, char **env_ptr);
void		remove_env_var(const char *var_name);
int			exec_unset(char **args);

/**
 * @file create_heredoc.c
 * @brief Contains functions for handling heredoc creation.
 */
// CREATE HEREDOC
int			setup_and_restore_signals(struct sigaction *sa,
				struct sigaction *old_sa);
pid_t		setup_pipe_and_fork(int pipefd[2], struct sigaction *old_sa);
void		setup_child_process(int write_fd, const char *delimiter);
int			create_heredoc(const char *delimiter);

/**
 * @file execute_pipe.c
 * @brief Contains functions for handling pipeline execution.
 */
// EXECUTE PIPE
void		execute_child_process(int prev_pipe[2],
				int next_pipe[2], t_cmd *current);
int			fork_and_execute(int *pipes[2], t_cmd *current,
				pid_t *child_pids, int *i);
int			process_pipeline_command(t_pipeline_context *ctx, int *pipes[2]);
int			process_all_pipeline_commands(t_pipeline_context *ctx,
				int *pipes[2], struct sigaction *old_sa);
int			execute_pipeline(t_cmd *pipeline);

/**
 * @file exit_status.c
 * @brief Contains functions for managing the shell's exit status.
 */
// EXIT STATUS
void		set_exit_status(int status);
int			get_exit_status(void);

/**
 * @file get_cmd_path.c
 * @brief Contains functions for resolving command paths.
 */
// GET CMD PATH
char		*check_path_for_cmd(const char *dir_path, const char *cmd);
char		**get_path_directories(void);
char		*check_direct_path(char *cmd);
char		*get_cmd_path(char *cmd);
void		execute_command(t_cmd *cmd, int pipe_in, int pipe_out);

/**
 * @file handle_commands.c
 * @brief Contains functions for handling command execution.
 */
// HANDLE COMMANDS
void		execute_non_builtin(t_cmd *cmd, char **environ);
void		handle_child_exit(pid_t pid, struct sigaction *old_sa);
void		setup_and_execute_child(t_cmd *cmd, int pipe_in, int pipe_out);
void		handle_builtin_in_parent(t_cmd *pipeline);
int			handle_command_pipeline(char *command, int *should_exit);

// HANDLE HEREDOC
void		write_line_to_pipe(int write_fd, const char *line,
				int quoted_delimiter);
int			is_quoted_delimiter(const char *delimiter);
void		handle_child_process(int write_fd, const char *delimiter);
int			handle_parent_process(pid_t pid, int read_fd, int *was_signaled);

/**
 * @file handle_parse_args.c
 * @brief Contains functions for parsing command arguments.
 */
// HANDLE PARSE ARGS
t_redir		*handle_redir_error(t_parse *p, t_list **args, t_redir *redirs);
int			handle_argument_token(t_parse *p, t_list **args, t_redir *redirs);
int			handle_redirection_token(t_parse *p, t_list **args,
				t_redir **redir_tail, t_redir *redirs);
t_cmd		*handle_syntax_error(t_parse *p, t_list *args, t_redir *redirs,
				char *error_message);

/**
 * @file handle_parse.c
 * @brief Contains functions for parsing input.
 */
// HANDLE PARSE
void		handle_unmatched_quote(t_parse *p);
void		handle_quotes(t_parse *p, char quote);
char		*handle_quoted_segment(t_parse *p,
				char *value, t_quote_state *quote_state);
char		*handle_end_of_quote(t_parse *p, char *value,
				size_t *segment_start, int *in_quote);
char		*handle_unquoted_segment(t_parse *p, char *value,
				size_t *segment_start);

/**
 * @file handle_pipe.c
 * @brief Contains functions for managing pipes in command execution.
 */
// HANDLE PIPE
int			wait_for_children(pid_t *child_pids, int cmd_count,
				int *was_signaled);
void		close_and_update_pipes(int prev_pipe[2], int next_pipe[2]);
void		close_remaining_pipes(int prev_pipe[2]);
int			handle_heredoc_redirections(t_redir *redirections);
void		handle_last_command(t_cmd *pipeline, int last_status,
				pid_t *child_pids);

/**
 * @file handle_redirect.c
 * @brief Contains functions for managing redirections.
 */
// HANDLE REDIRECT
int			open_redirection_fd(t_redir *redir);
int			duplicate_fd(int fd, int target);
int			handle_file_redirections(t_redir *redirections);
int			handle_single_redirection(t_redir *redir);
int			handle_redirections(int pipe_in,
				int pipe_out, t_redir *redirections);

/**
 * @file handle_special.c
 * @brief Contains functions for handling special tokens and cases.
 */
// HANDLE SPECIAL
void		assign_value(t_parse *p, enum e_token t, char *v, int n);
void		handle_file_descriptor(t_parse *p);
void		handle_output_redirection(t_parse *p);
void		handle_input_redirection(t_parse *p);
void		handle_special(t_parse *p);

/**
 * @file init.c
 * @brief Contains functions for initializing shell components.
 */
// INIT
void		init_parser(t_parse *p, char *input);
void		init_word_and_quote_states(t_parse *p,
				t_word_state *word_state, t_quote_state *quote_state);
int			initialize_pipeline_resources(t_cmd *pipeline, int prev_pipe[2],
				int next_pipe[2], pid_t **child_pids);
int			initialize_pipeline(t_cmd *pipeline,
				t_pipeline_context *ctx, struct sigaction *old_sa);

/**
 * @file input.c
 * @brief Contains functions for handling user input and variable expansion.
 */
// INPUT
char		*expand_variable(const char *input, size_t *i);
char		*expand_and_append_variable(const char *input,
				size_t *i, char *result);
char		*expand_variables(const char *input);
void		exec_external_command(t_cmd *cmd);
int			handle_redirection(t_redir *current, int fd);

/**
 * @file main.c
 * @brief Contains the main entry point and core logic for the shell.
 */
// MAIN
t_cmd		*parse_and_validate_pipeline(char *command, t_parse *parser);
void		free_pipeline(t_cmd *pipeline);
void		execute_command(t_cmd *cmd, int pipe_in, int pipe_out);
int			process_commands(char **commands, int *should_exit);
int			handle_shell_loop(char **env_copy,
				char **original_environ, int *exit_status);

/**
 * @file parse_args.c
 * @brief Contains functions for parsing command arguments.
 */
// PARSE ARGS
void		process_argument(t_parse *p, t_list **args);
t_redir		*process_redirection(t_parse *p, t_list **args, t_redir *redirs);
t_cmd		*create_command(t_list *args, t_redir *redirs);
int			process_tokens(t_parse *p, t_token_state *state);
t_cmd		*parse_args(t_parse *p);

/**
 * @file parse_pipeline.c
 * @brief Contains functions for parsing pipelines.
 */
// PARSE PIPELINE
int			is_pipe_or_semicolon(t_parse *p);
int			handle_cmd_result(t_parse *p, t_cmd **head,
				t_cmd *cmd, t_cmd ***curr);
int			check_pipe_error(t_parse *p, t_cmd *head);
int			check_final_error(t_parse *p, t_cmd *head);
t_cmd		*parse_pipeline(t_parse *p);

/**
 * @file parsers.c
 * @brief Contains utility functions for parsing input.
 */
// PARSERS
int			advance_to_closing_quote(t_parse *p, char quote);
char		*add_segment(char *value, const char *input, size_t start,
				size_t end);
char		*process_quoted_segment(t_parse *p,
				char *value, t_quote_state *quote_state);
void		next_token(t_parse *p);
void		handle_word(t_parse *p);
char		**build_expanded_args(t_list *args);

// PIPE SIGNALS
int			setup_signal_handling(struct sigaction *sa,
				struct sigaction *old_sa);
int			manage_signal_handling(struct sigaction *old_sa, int was_signaled);
int			create_pipe(int next_pipe[2], pid_t *child_pids);

/**
 * @file signals.c
 * @brief Contains functions for handling signals.
 */
// SIGNALS
void		handle_sigint(int sig);
void		setup_parent_signals(void);
void		setup_child_signals(void);
void		setup_heredoc_signals(void);

/**
 * @file utils.c
 * @brief Contains utility functions used throughout the shell.
 */
// UTILS
void		ft_free_array(char **args);
int			ft_isspace(int c);
void		next_char(t_parse *p);
int			is_special_char(char c);
char		**list_to_array(t_list *lst);
void		free_cmd(t_cmd *cmd);
int			ft_strcmp(const char *s1, const char *s2);
int			ft_isnumber(const char *str);
int			is_valid_var_name(const char *name);
void		print_export_declarations(void);
size_t		ft_strlen_size(const char *str);
char		*ft_strjoin3(const char *s1, const char *s2, const char *s3);
char		**ft_array_append(char **array, char *new_element);
void		free_arg(void *arg);
char		*ft_strjoin_free(char *s1, const char *s2);
char		*ft_strjoin_char(char *str, char c);
size_t		ft_cmd_size(t_cmd *pipeline);
void		free_redirections(t_redir *redirs);
void		free_env_copy(char **env_copy);
void		syntax_error(char *token);
char		*parse_fd(t_parse *p);
void		update_quote_state(char c, int *in_quote, char *quote_char);
void		process_quote_state(const char *str, int *in_quote,
				char *quote_char);
void		add_substring(char ***result, int *count, const char *start,
				const char *end);
void		handle_delimiter(const char **str, const char **start,
				char ***result, int *count);
const char	*skip_escaped(const char *str, char delim);
void		finalize_split(const char *start, const char *str,
				char ***result, int *count);
char		**split_with_quotes(const char *str, char delim);
char		*process_escapes(char *str);
int			is_redirection(t_token type);
void		skip_whitespace(t_parse *p);
void		handle_error(char *message);
char		**read_and_split_input(char **env_copy, int *should_exit);
int			cleanup_on_failure(pid_t **child_pids, int exit_code);
void		free_commands(char **commands);

#endif
