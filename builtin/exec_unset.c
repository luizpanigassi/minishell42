/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_unset.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: luinasci <luinasci@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/21 14:51:59 by luinasci          #+#    #+#             */
/*   Updated: 2025/04/21 14:52:14 by luinasci         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/**
 * @brief Executes the 'unset' builtin to remove environment variables.
 * @param args Array of arguments (e.g., ["unset", "VAR"]).
 * @return 0 on success, 1 if any invalid variable name is encountered.
 */
int exec_unset(char **args)
{
	extern char **environ;
	int ret;
	int i;

	if (!args[1])
		return (0);

	ret = 0;
	i = 1;
	while (args[i])
	{
		if (!is_valid_var_name(args[i]))
		{
			ft_putstr_fd("minishell: unset: '", STDERR_FILENO);
			ft_putstr_fd(args[i], STDERR_FILENO);
			ft_putstr_fd("': not a valid identifier\n", STDERR_FILENO);
			ret = 1;
			i++;
			continue;
		}

		char **env_ptr = environ;
		while (*env_ptr)
		{
			char *eq = ft_strchr(*env_ptr, '=');
			size_t var_len = eq ? eq - *env_ptr : ft_strlen(*env_ptr);

			if (ft_strncmp(*env_ptr, args[i], var_len) == 0 && var_len == ft_strlen_size(args[i]))
			{
				free(*env_ptr);
				char **ptr = env_ptr;
				while (*ptr)
				{
					*ptr = *(ptr + 1);
					ptr++;
				}
				break;
			}
			env_ptr++;
		}
		i++;
	}
	return (ret);
}
