/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: luinasci <luinasci@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/03 18:15:03 by luinasci          #+#    #+#             */
/*   Updated: 2025/04/16 14:47:35 by luinasci         ###   ########.fr       */
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
	char	curr_char;
	char	*token_value;
	char	*input;
	int		pos;
	int		redir_fd;
	t_token	token_type;
}	t_parse;

// Redirection structure
// This structure is used to represent redirections in the command line.
// It can represent input/output redirections, appends, and heredocs
// T_REDIR_IN/OUT, Target file, multiple redirects, quoted tracking, and fd
typedef struct s_redir {
	t_token			type;
	char			*filename;
	struct s_redir	*next;
	int				heredoc_quoted;
	int				fd;
}	t_redir;

// Command structure
// This structure represents a command in the shell.
// Command arguments, list of redirs and pipe structure
typedef struct s_cmd {
	char			**args;
	t_redir			*redirections;
	struct s_cmd	*next;
}	t_cmd;

// List structure
// This structure is used to represent a linked list of arguments.
// Each node contains a string value and a token type
typedef struct s_arg {
	char	*value;
	t_token	type;
}	t_arg;

/*
** Global variable to store the exit status of commands
** volatile sig_atomic_t ensures safe access in signal handlers
*/
extern volatile sig_atomic_t	g_exit_status;

// BUILTIN FUNCTIONS
int		is_builtin(char **args);
int		exec_cd(char **args);
int		exec_exit(char **args);
int		exec_echo(char **args);
int		exec_builtin(char **args);
int		exec_env(char **args);
int		exec_pwd(char **args);
int		exec_export(char **args);
int		exec_unset(char **args);
char	**ft_copy_env(char **env);
void	update_env_var(char *var, char *value);
void	ensure_var_exported(char *var_name);

// EXIT STATUS FUNCTIONS
void	set_exit_status(int status);
int		get_exit_status(void);

// INPUT FUNCTIONS
void	handle_error(char *message);
void	exec_external_command(t_cmd *cmd);
char	*get_cmd_path(char *cmd);
char	*expand_variables(const char *input);
int		handle_redirection(t_redir *current, int fd);

// MAIN HELPER FUNCTIONS
void	free_pipeline(t_cmd *pipeline);
void	execute_command(t_cmd *cmd, int pipe_in, int pipe_out);
int		execute_pipeline(t_cmd *pipeline);
int		handle_redirections(int pipe_in, int pipe_out, t_redir *redirections);

// PARSING FUNCTIONS
void	init_parser(t_parse *p, char *input);
void	skip_whitespace(t_parse *p);
void	handle_quotes(t_parse *p, char quote);
void	handle_word(t_parse *p);
void	handle_special(t_parse *p);
void	next_token(t_parse *p);
char	**build_expanded_args(t_list *args);
int		create_heredoc(const char *delimiter);
int		is_redirection(t_token type);
t_cmd	*parse_pipeline(t_parse *p);
t_cmd	*parse_args(t_parse *p);

// SIGNALS FUNCTIONS
void	handle_sigint(int sig);
void	setup_parent_signals(void);
void	setup_child_signals(void);

// UTILS
int		ft_isspace(int c);
int		is_special_char(char c);
int		ft_strcmp(const char *s1, const char *s2);
int		ft_isnumber(const char *str);
int		is_valid_var_name(const char *name);
size_t	ft_strlen_size(const char *s);
size_t	ft_cmd_size(t_cmd *pipeline);
void	print_export_declarations(void);
void	free_arg(void *arg);
void	free_cmd(t_cmd *cmd);
void	next_char(t_parse *p);
void	ft_free_array(char **array);
void	free_redirections(t_redir *redirs);
void	free_env_copy(char **env_copy);
void	syntax_error(char *token);
char	*parse_fd(t_parse *p);
char	**split_with_quotes(const char *str, char delim);
char	*ft_strjoin_free(char *s1, const char *s2);
char	*ft_strjoin_char(char *str, char c);
char	*ft_strjoin3(const char *s1, const char *s2, const char *s3);
char	**ft_array_append(char **array, char *new_element);
char	**list_to_array(t_list *lst);

#endif
