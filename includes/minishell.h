/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: luinasci <luinasci@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/11 17:18:18 by jcologne          #+#    #+#             */
/*   Updated: 2025/03/27 18:13:06 by luinasci         ###   ########.fr       */
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

# define CMD_NOT_FOUND 127
# define PERM_DENIED 126
# define SYNTAX_ERROR 2

// Parsing structure
typedef enum e_token
{
	T_WORD,
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

// INPUT
void	handle_error(char *message);
char	*get_cmd_path(char *cmd);

// BUILTIN
int		is_builtin(char **args);
int		exec_cd(char **args);
int		exec_exit(char **args);
int		exec_echo(char **args);

// SIGNALS
void	handle_sigint(int sig);
void	setup_parent_signals(void);
void	setup_child_signals(void);

// PARSING
void	init_parser(t_parse *p, char *input);
void	next_token(t_parse *p);
char	**parse_args(t_parse *p);

// UTILS
void	ft_free_array(char **array);

// Helper functions for parsing
int		ft_isspace(int c);
void	next_char(t_parse *p);
int		is_special_char(char c);
char	**list_to_array(t_list *lst);

#endif
