/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_printf.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: luinasci <luinasci@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/19 17:56:08 by luinasci          #+#    #+#             */
/*   Updated: 2024/11/19 17:57:22 by luinasci         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_printf.h"

int	ft_printf(const char *format, ...)
{
	int				count;
	va_list			args;

	if (!format)
		return (-1);
	count = 0;
	va_start(args, format);
	while (*format != '\0')
	{
		if (*format == '%')
		{
			format++;
			count += ft_handle_everything(args, *format);
		}
		else
			count += ft_putchar(*format);
		format++;
	}
	va_end(args);
	return (count);
}

int	ft_handle_everything(va_list args, char identifier)
{
	int	count;

	count = 0;
	if (identifier == 'd' || identifier == 'i')
		count += handle_d(args);
	else if (identifier == 'u')
		count += handle_u(args);
	else if (identifier == 'x')
		count += handle_x(args);
	else if (identifier == 'X')
		count += handle_big_x(args);
	else if (identifier == 'p')
		count += handle_p(args);
	else if (identifier == 's')
		count += handle_s(args);
	else if (identifier == 'c')
		count += handle_c(args);
	else if (identifier == '%')
		count += ft_putchar('%');
	return (count);
}

int	ft_putchar(char c)
{
	write(1, &c, 1);
	return (1);
}
