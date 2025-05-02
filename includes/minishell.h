/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: luinasci <luinasci@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/03 18:15:03 by luinasci          #+#    #+#             */
/*   Updated: 2025/05/02 17:37:58 by luinasci         ###   ########.fr       */
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

// Parsing structure
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

typedef struct s_redir {
	t_token			type;
	char			*filename;
	struct s_redir	*next;
	int				heredoc_quoted;
	int				fd;
}	t_redir;

typedef struct s_cmd {
	char			**args;
	t_redir			*redirections;
	struct s_cmd	*next;
}	t_cmd;

typedef struct s_arg {
	char	*value;
	t_token	type;
}	t_arg;

typedef struct s_export_params {
	int		*ret;
	int		*i;
	char	*arg;
}	t_export_params;

typedef struct s_exec_vars {
	int		prev_pipe[2];
	int		next_pipe[2];
	pid_t	*child_pids;
	pid_t	pid;
	t_cmd	*current;
	int		last_status;
}	t_exec_vars;

typedef struct s_token_state
{
	t_list		**args;
	t_redir		**redirs;
	t_redir		***redir_tail;
	int			*has_args;
}	t_token_state;

/*
** Global variable to store the exit status of commands
** volatile sig_atomic_t ensures safe access in signal handlers
*/
extern volatile sig_atomic_t	g_exit_status;

// BUILTIN
int		is_builtin(char **tokens);
int		exec_builtin(char **args);
char	**ft_copy_env(char **original);
void	update_env_var(char *var, char *value);
void	ensure_var_exported(char *var_name);

// EXEC CD
int		exec_cd(char **args);
char	*resolve_cd_path(char **args, char *oldpwd);
int		handle_cd_arguments(char **args, char *oldpwd);

// EXEC ECHO
int		exec_echo(char **args);

// EXEC ENV
int		exec_env(char **args);

// EXEC EXIT
int		exec_exit(char **args);

// EXEC EXPORT
int		exec_export(char **args);
int		handle_invalid_identifier(char *var_name, char *value,
			t_export_params params);
int		handle_export_argument(char *arg, int *i, int *ret);

// EXEC PWD
int		exec_pwd(char **args);

// EXEC UNSET
int		handle_unset_error(char *arg);
void	shift_environment(char **env_ptr);
void	remove_env_var(const char *var_name);
int		exec_unset(char **args);

// CREATE HEREDOC
int		create_heredoc(const char *delimiter);
void	handle_child_process(int write_fd, const char *delimiter);
int		handle_parent_process(pid_t pid, int read_fd);

// EXIT STATUS
void	set_exit_status(int status);
int		get_exit_status(void);

// GET CMD PATH
char	*check_path_for_cmd(const char *dir_path, const char *cmd);
char	**get_path_directories(void);
char	*check_direct_path(char *cmd);
char	*get_cmd_path(char *cmd);

// HANDLE PIPE
int		execute_pipeline(t_cmd *pipeline);

// HANDLE SPECIAL
void	assign_value(t_parse *p, enum e_token t, char *v, int n);
void	handle_file_descriptor(t_parse *p);
void	handle_output_redirection(t_parse *p);
void	handle_input_redirection(t_parse *p);
void	handle_special(t_parse *p);

// INIT PARSER
void	init_parser(t_parse *p, char *input);

// INPUT
void	handle_error(char *message);
char	*expand_variable(const char *input, size_t *i);
char	*expand_variables(const char *input);
void	exec_external_command(t_cmd *cmd);
int		handle_redirection(t_redir *current, int fd);

// MAIN
void	free_pipeline(t_cmd *pipeline);
void	execute_command(t_cmd *cmd, int pipe_in, int pipe_out);
int		handle_redirections(int pipe_in, int pipe_out, t_redir *redirections);

// PARSE ARGS
void	process_argument(t_parse *p, t_list **args);
t_redir	*handle_redir_error(t_parse *p, t_list **args, t_redir *redirs);
t_redir	*process_redirection(t_parse *p, t_list **args, t_redir *redirs);
t_cmd	*create_command(t_list *args, t_redir *redirs);
t_cmd	*parse_args(t_parse *p);

// PARSE PIPELINE
t_cmd	*parse_pipeline(t_parse *p);

// PARSERS
void	skip_whitespace(t_parse *p);
void	handle_quotes(t_parse *p, char quote);
void	handle_word(t_parse *p);
void	next_token(t_parse *p);
int		is_redirection(t_token type);
char	**build_expanded_args(t_list *args);

// SIGNALS
void	handle_sigint(int sig);
void	setup_parent_signals(void);
void	setup_child_signals(void);
void	setup_heredoc_signals(void);

// UTILS
void	ft_free_array(char **args);
int		ft_isspace(int c);
void	next_char(t_parse *p);
int		is_special_char(char c);
char	**list_to_array(t_list *lst);
void	free_cmd(t_cmd *cmd);
int		ft_strcmp(const char *s1, const char *s2);
int		ft_isnumber(const char *str);
int		is_valid_var_name(const char *name);
void	print_export_declarations(void);
size_t	ft_strlen_size(const char *str);
char	*ft_strjoin3(const char *s1, const char *s2, const char *s3);
char	**ft_array_append(char **array, char *new_element);
void	free_arg(void *arg);
char	*ft_strjoin_free(char *s1, const char *s2);
char	*ft_strjoin_char(char *str, char c);
size_t	ft_cmd_size(t_cmd *pipeline);
void	free_redirections(t_redir *redirs);
void	free_env_copy(char **env_copy);
void	syntax_error(char *token);
char	*parse_fd(t_parse *p);
void	update_quote_state(char c, int *in_quote, char *quote_char);
void	add_substring(char ***result, int *count, const char *start,
			const char *end);
void	handle_delimiter(const char **str, const char **start,
			char ***result, int *count);
char	**split_with_quotes(const char *str, char delim);
char	*process_escapes(char *str);

#endif
