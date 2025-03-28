/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: luinasci <luinasci@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/11 15:43:31 by jcologne          #+#    #+#             */
/*   Updated: 2025/03/28 16:01:53 by luinasci         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

extern char **environ; // Add this for environment access

void free_args(char **args)
{
	int i;

	if (!args)
		return;
	i = 0;
	while (args[i])
	{
		free(args[i]);
		i++;
	}
	free(args);
}

int main(void)
{
	char *input;
	t_cmd *cmd;
	pid_t pid;
	t_parse parser;
	int builtin_ret;

	setup_parent_signals();
	while (1)
	{
		input = readline("$minishell> ");
		if (!input)
		{
			ft_putstr_fd("exit\n", STDOUT_FILENO);
			break;
		}
		add_history(input);

		init_parser(&parser, input);
		cmd = parse_args(&parser);

		if (!cmd || !cmd->args[0])
		{
			free(input);
			free_cmd(cmd);
			continue;
		}

		builtin_ret = is_builtin(cmd->args); // Pass cmd->args
		if (builtin_ret)
		{
			if (builtin_ret == 2) // Exit command
			{
				free_cmd(cmd);
				free(input);
				exit(0);
			}
			free_cmd(cmd);
			free(input);
			continue;
		}

		pid = fork();
		if (pid == -1)
		{
			perror("fork");
			free_cmd(cmd);
			free(input);
			continue;
		}
		else if (pid == 0) // Child
		{
			setup_child_signals();
			t_redir *redir = cmd->redirections;
			while (redir)
			{
				int fd;
				if (redir->type == T_REDIR_OUT)
				{
					fd = open(redir->filename, O_WRONLY | O_CREAT | O_TRUNC, 0644);
					dup2(fd, STDOUT_FILENO);
					close(fd);
				}
				else if (redir->type == T_APPEND)
				{
					fd = open(redir->filename, O_WRONLY | O_CREAT | O_APPEND, 0644);
					dup2(fd, STDOUT_FILENO);
					close(fd);
				}
				else if (redir->type == T_REDIR_IN)
				{
					fd = open(redir->filename, O_RDONLY);
					dup2(fd, STDIN_FILENO);
					close(fd);
				}
				else if (redir->type == T_HEREDOC)
				{
					int tmp_fd = create_heredoc(redir->filename);
					dup2(tmp_fd, STDIN_FILENO);
					close(tmp_fd);
				}
				redir = redir->next;
			}
			// Execute command
			char *path = get_cmd_path(cmd->args[0]);
			if (!path)
			{
				ft_putstr_fd("minishell: command not found\n", STDERR_FILENO);
				exit(CMD_NOT_FOUND);
			}
			execve(path, cmd->args, environ);
			exit(EXIT_FAILURE); // If execve fails
		}
		else // Parent
		{
			waitpid(pid, NULL, 0);
			free_cmd(cmd);
			free(input);
		}
	}
	return (0);
}
