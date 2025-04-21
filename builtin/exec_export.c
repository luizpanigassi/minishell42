/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_export.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: luinasci <luinasci@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/21 14:50:18 by luinasci          #+#    #+#             */
/*   Updated: 2025/04/21 15:37:48 by luinasci         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/**
 * @brief Executes the 'export' builtin to set environment variables.
 * @param args Array of arguments (e.g., ["export", "VAR=value"]).
 * @return 0 on success, 1 if any invalid variable name is encountered.
 */
int	exec_export(char **args)
{
	int	ret;
	int	i;

	if (!args[1])
		return (print_export_declarations(), 0);
	ret = 0;
	i = 1;
	while (args[i])
	{
		if (handle_export_argument(args[i], &i, &ret))
			continue ;
		i++;
	}
	return (ret);
}

int	handle_invalid_identifier(char *var_name, char *value,
		t_export_params params)
{
	if (is_valid_var_name(var_name))
		return (0);
	ft_putstr_fd("minishell: export: '", STDERR_FILENO);
	ft_putstr_fd(params.arg, STDERR_FILENO);
	ft_putstr_fd("': not a valid identifier\n", STDERR_FILENO);
	*params.ret = 1;
	free(var_name);
	free(value);
	(*params.i)++;
	return (1);
}

int	handle_export_argument(char *arg, int *i, int *ret)
{
	char			*eq_pos;
	char			*var_name;
	char			*value;
	t_export_params	params;

	params = (t_export_params){ret, i, arg};
	eq_pos = ft_strchr(arg, '=');
	var_name = NULL;
	value = NULL;
	if (eq_pos)
	{
		var_name = ft_substr(arg, 0, eq_pos - arg);
		value = ft_strdup(eq_pos + 1);
	}
	else
		var_name = ft_strdup(arg);
	if (handle_invalid_identifier(var_name, value, params))
		return (1);
	if (eq_pos)
		update_env_var(var_name, value);
	else
		ensure_var_exported(var_name);
	return (0);
}
