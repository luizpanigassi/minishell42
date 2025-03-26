/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_printf_utils.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: luinasci <luinasci@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/19 17:55:57 by luinasci          #+#    #+#             */
/*   Updated: 2024/11/19 17:57:23 by luinasci         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_printf.h"

int	ft_print_str(char *str)
{
	int	count;

	if (!str)
		return (ft_print_str("(null)"));
	count = 0;
	while (*str)
	{
		count += ft_putchar(*str);
		str++;
	}
	return (count);
}

int	ft_print_nbr(long nbr)
{
	int		count;
	char	c;

	count = 0;
	if (nbr < 0)
	{
		count += ft_putchar('-');
		nbr = -nbr;
	}
	if (nbr >= 10)
	{
		count += ft_print_nbr(nbr / 10);
		count += ft_print_nbr(nbr % 10);
	}
	else
	{
		c = nbr + '0';
		count += ft_putchar(c);
	}
	return (count);
}

int	ft_print_unbr(unsigned int nbr)
{
	int		count;
	char	c;

	count = 0;
	if (nbr >= 10)
	{
		count += ft_print_nbr(nbr / 10);
		count += ft_print_nbr(nbr % 10);
	}
	else
	{
		c = nbr + '0';
		count += ft_putchar(c);
	}
	return (count);
}

int	ft_print_ptr(va_list args)
{
	int				count;
	unsigned long	ptr;

	count = 0;
	ptr = va_arg(args, unsigned long);
	if (ptr == 0)
		count += ft_print_str("(nil)");
	else
	{
		count += ft_print_str("0x");
		count += ft_print_hex(ptr, "0123456789abcdef");
	}
	return (count);
}

int	ft_print_hex(unsigned long n, const char *base)
{
	int			count;
	char		c;

	count = 0;
	if (n >= 16)
	{
		count += ft_print_hex(n / 16, base);
		count += ft_print_hex(n % 16, base);
	}
	else
	{
		c = base[n];
		count += ft_putchar(c);
	}
	return (count);
}
