/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_handlers.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: luinasci <luinasci@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/19 17:55:30 by luinasci          #+#    #+#             */
/*   Updated: 2024/11/19 17:57:25 by luinasci         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_printf.h"

int	handle_s(va_list args)
{
	char	*str;

	str = va_arg(args, char *);
	if (!str)
		return (ft_print_str("(null)"));
	return (ft_print_str(str));
}

int	handle_d(va_list args)
{
	long	number;

	number = va_arg(args, int);
	return (ft_print_nbr(number));
}

int	handle_u(va_list args)
{
	unsigned int	u_number;

	u_number = va_arg(args, unsigned int);
	return (ft_print_unbr(u_number));
}

int	handle_p(va_list args)
{
	unsigned long	ptr;

	ptr = va_arg(args, unsigned long);
	if (ptr == 0)
		return (ft_print_str("(nil)"));
	return (ft_print_str("0x") + ft_print_hex(ptr, "0123456789abcdef"));
}
