/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: luinasci <luinasci@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/11 15:43:31 by jcologne          #+#    #+#             */
/*   Updated: 2025/05/07 18:15:01 by luinasci         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

volatile sig_atomic_t	g_exit_status = 0;

/**
 * @brief Frees a command pipeline and associated resources.
 * @param pipeline Linked list of command structures.
 */
void	free_pipeline(t_cmd *pipeline)
{
	t_cmd	*current;
	t_redir	*redir;
	t_redir	*tmp_redir;

	while (pipeline)
	{
		current = pipeline;
		pipeline = pipeline->next;
		if (current->args)
			ft_free_array(current->args);
		redir = current->redirections;
		while (redir)
		{
			tmp_redir = redir;
			redir = redir->next;
			free(tmp_redir->filename);
			free(tmp_redir);
		}
		free(current);
	}
}

/**
 * @brief Parses and validates the command pipeline.
 * @param command The command string to process.
 * @param parser Pointer to the parser structure.
 * @return Parsed pipeline or NULL if an error occurs.
 */
t_cmd	*parse_and_validate_pipeline(char *command, t_parse *parser)
{
	char	*trimmed_cmd;
	t_cmd	*pipeline;

	trimmed_cmd = ft_strtrim(command, " \t\n");
	if (!trimmed_cmd || *trimmed_cmd == '\0')
	{
		free(trimmed_cmd);
		return (NULL);
	}
	init_parser(parser, trimmed_cmd);
	pipeline = parse_pipeline(parser);
	free(trimmed_cmd);
	if (parser->syntax_error)
	{
		set_exit_status(SYNTAX_ERROR);
		free_pipeline(pipeline);
		return (NULL);
	}
	return (pipeline);
}

/**
 * @brief Processes a list of commands and handles pipelines.
 * @param commands Array of command strings.
 * @param should_exit Pointer to the exit flag.
 * @return 1 if a syntax error occurs, 0 otherwise.
 */
int	process_commands(char **commands, int *should_exit)
{
	int	syntax_error_flag;
	int	i;

	i = 0;
	syntax_error_flag = 0;
	while (commands && commands[i] && !syntax_error_flag && !(*should_exit))
	{
		if (handle_command_pipeline(commands[i], should_exit))
		{
			syntax_error_flag = 1;
		}
		i++;
	}
	return (syntax_error_flag);
}

/**
 * @brief Handles the readline loop and command processing.
 * @param env_copy Copy of the environment variables.
 * @param original_environ Original environment variables.
 * @param exit_status Pointer to the exit status variable.
 * @return 1 if the shell should exit, 0 otherwise.
 */
int	handle_shell_loop(char **env_copy,
	char **original_environ, int *exit_status)
{
	extern char	**environ;
	char		**commands;
	int			should_exit;
	int			syntax_error_flag;

	should_exit = 0;
	while (!should_exit)
	{
		commands = read_and_split_input(env_copy, &should_exit);
		if (!commands)
			continue ;
		syntax_error_flag = process_commands(commands, &should_exit);
		free_commands(commands);
		if (syntax_error_flag)
		{
			set_exit_status(SYNTAX_ERROR);
			*exit_status = SYNTAX_ERROR;
		}
	}
	environ = original_environ;
	rl_clear_history();
	return (should_exit);
}

/**
 * @brief Main shell entry point.
 * @note Manages readline loop, signal handling, and cleanup.
 */
int	main(void)
{
	extern char	**environ;
	char		**original_environ;
	char		**env_copy;
	int			exit_status;

	original_environ = environ;
	env_copy = ft_copy_env(environ);
	exit_status = 0;
	environ = env_copy;
	setup_parent_signals();
	if (handle_shell_loop(env_copy, original_environ, &exit_status))
		return (exit_status);
	return (exit_status);
}
