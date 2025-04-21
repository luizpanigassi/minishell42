/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_unset.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: luinasci <luinasci@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/21 14:51:59 by luinasci          #+#    #+#             */
/*   Updated: 2025/04/21 15:48:05 by luinasci         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static int	handle_unset_error(char *arg)
{
	if (is_valid_var_name(arg))
		return (0);
	ft_putstr_fd("minishell: unset: '", STDERR_FILENO);
	ft_putstr_fd(arg, STDERR_FILENO);
	ft_putstr_fd("': not a valid identifier\n", STDERR_FILENO);
	return (1);
}

static void	shift_environment(char **env_ptr)
{
	char	**ptr;

	ptr = env_ptr;
	while (*ptr)
	{
		*ptr = *(ptr + 1);
		ptr++;
	}
}

static void	remove_env_var(const char *var_name)
{
	extern char	**environ;
	char		**env_ptr;
	char		*eq;
	size_t		var_len;
	size_t		name_len;

	env_ptr = environ;
	name_len = ft_strlen(var_name);
	while (*env_ptr)
	{
		eq = ft_strchr(*env_ptr, '=');
		if (eq)
			var_len = eq - *env_ptr;
		else
			var_len = ft_strlen(*env_ptr);
		if (var_len == name_len && ft_strncmp(*env_ptr, var_name, var_len) == 0)
		{
			free(*env_ptr);
			shift_environment(env_ptr);
			break ;
		}
		env_ptr++;
	}
}

/**
 * @brief Executes the 'unset' builtin to remove environment variables.
 * @param args Array of arguments (e.g., ["unset", "VAR"]).
 * @return 0 on success, 1 if any invalid variable name is encountered.
 */
int	exec_unset(char **args)
{
	int	ret;
	int	i;

	if (!args[1])
		return (0);
	ret = 0;
	i = 1;
	while (args[i])
	{
		if (handle_unset_error(args[i]))
			ret = 1;
		else
			remove_env_var(args[i]);
		i++;
	}
	return (ret);
}
