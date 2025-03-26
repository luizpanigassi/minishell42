/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_printf.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: luinasci <luinasci@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/19 17:56:20 by luinasci          #+#    #+#             */
/*   Updated: 2024/11/19 17:57:20 by luinasci         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef FT_PRINTF_H
# define FT_PRINTF_H

# include <stddef.h>
# include <stdarg.h>
# include <stdlib.h>
# include <unistd.h>

int			ft_printf(const char *format, ...);
int			ft_handle_everything(va_list args, char identifier);
int			ft_putchar(char c);
int			ft_print_str(char *str);
int			ft_print_nbr(long nbr);
int			ft_print_unbr(unsigned int nbr);
int			ft_print_ptr(va_list args);
int			ft_print_hex(unsigned long n, const char *base);
int			handle_s(va_list args);
int			handle_d(va_list args);
int			handle_u(va_list args);
int			handle_p(va_list args);
int			handle_x(va_list args);
int			handle_big_x(va_list args);
int			handle_c(va_list args);

#endif
