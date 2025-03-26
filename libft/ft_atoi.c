/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_atoi.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: luinasci <luinasci@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/10 17:43:44 by luinasci          #+#    #+#             */
/*   Updated: 2024/10/29 14:53:45 by luinasci         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

static void	skip_whitespace(const char **str, int *sign)
{
	while (**str == ' ' || **str == '\t' || **str == '\n'
		|| **str == '\r' || **str == '\v' || **str == '\f')
	{
		(*str)++;
	}
	if (**str == '-')
	{
		*sign = -1;
		(*str)++;
	}
	else if (**str == '+')
	{
		(*str)++;
	}
}

int	ft_atoi(const char *str)
{
	int				sign;
	unsigned int	result;

	sign = 1;
	result = 0;
	skip_whitespace(&str, &sign);
	while (*str >= '0' && *str <= '9')
	{
		if (result > (unsigned int)(INT_MAX - (*str - '0')) / 10)
		{
			if (sign == 1)
			{
				return (INT_MAX);
			}
			return (INT_MIN);
		}
		result = result * 10 + (*str++ - '0');
	}
	if (sign == -1)
	{
		return (-((int)result));
	}
	return ((int)result);
}
