/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jcologne <jcologne@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/11 17:18:18 by jcologne          #+#    #+#             */
/*   Updated: 2025/03/17 22:04:10 by jcologne         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#ifndef MINISHELL_H
# define MINISHELL_H

#include "../libft/libft.h"

# include <stdio.h>
# include <stdlib.h>
# include <unistd.h>
# include <string.h>
# include <sys/types.h>
# include <sys/wait.h>
# include <errno.h>
# include <readline/readline.h>
# include <readline/history.h>

//INPUT
void    handle_error(char *message);
char	*get_cmd_path(char *cmd);

//BUILTIN
int		is_builtin(char **args);
int		exec_cd(char **args);
int		exec_exit(char **args);
int		exec_echo(char **args);

#endif // MINISHELL_H