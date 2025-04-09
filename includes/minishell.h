/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: luinasci <luinasci@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/03 18:15:03 by luinasci          #+#    #+#             */
/*   Updated: 2025/04/09 15:13:40 by luinasci         ###   ########.fr       */
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
	T_EOF
}	t_token;

typedef struct s_parse
{
	char	*input;
	int		pos;
	char	curr_char;
	t_token	token_type;
	char	*token_value;
}	t_parse;

typedef struct s_redir {
	t_token			type;// T_REDIR_IN, T_REDIR_OUT, etc.
	char			*filename;// Target file
	struct s_redir	*next;// For multiple redirections
}	t_redir;

typedef struct s_cmd {
	char			**args; // Command arguments (e.g., ["ls", "-l"])
	t_redir			*redirections; // List of redirections
	struct s_cmd	*next; // For pipe
}	t_cmd;

typedef struct s_arg {
	char	*value;
	t_token	type;
}	t_arg;

/*
** Global variable to store the exit status of commands
** volatile sig_atomic_t ensures safe access in signal handlers
*/
extern volatile sig_atomic_t	g_exit_status;

// INPUT
void	handle_error(char *message);
char	*get_cmd_path(char *cmd);
char	*expand_variables(const char *input);
void	exec_external_command(t_cmd *cmd);

// BUILTIN
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
char	**build_expanded_args(t_list *args);

// SIGNALS
void	handle_sigint(int sig);
void	setup_parent_signals(void);
void	setup_child_signals(void);

// PARSING
void	init_parser(t_parse *p, char *input);
void	next_token(t_parse *p);
t_cmd	*parse_args(t_parse *p);
int		create_heredoc(const char *delimiter);
int		is_redirection(t_token type);
t_cmd	*parse_pipeline(t_parse *p);

// UTILS
void	ft_free_array(char **array);
void	free_cmd(t_cmd *cmd);
int		ft_strcmp(const char *s1, const char *s2);
char	**ft_array_append(char **array, char *new_element);
int		ft_isnumber(const char *str);
int		is_valid_var_name(const char *name);
void	print_export_declarations(void);
size_t	ft_strlen_size(const char *s);
char	**ft_array_append(char **array, char *new_element);
char	*ft_strjoin3(const char *s1, const char *s2, const char *s3);
void	free_arg(void *arg);
char	*ft_strjoin_free(char *s1, const char *s2);
char	*ft_strjoin_char(char *str, char c);
size_t	ft_cmd_size(t_cmd *pipeline);
void	free_redirections(t_redir *redirs);
void	free_env_copy(char **env_copy);

// Helper functions for parsing
int		ft_isspace(int c);
void	next_char(t_parse *p);
int		is_special_char(char c);
char	**list_to_array(t_list *lst);
int		handle_redirections(int pipe_in, int pipe_out, t_redir *redirections);

//Exit status
void	set_exit_status(int status);
int		get_exit_status(void);

#endif
