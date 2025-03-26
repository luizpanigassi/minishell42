/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_memchr.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: luinasci <luinasci@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/10 17:46:23 by luinasci          #+#    #+#             */
/*   Updated: 2024/10/29 14:55:07 by luinasci         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

void	*ft_memchr(void *str, int c, size_t n)
{
	unsigned char	*s;
	unsigned char	ch;
	size_t			i;

	i = 0;
	s = (unsigned char *)str;
	ch = (unsigned char)c;
	while (i < n)
	{
		if (s[i] == ch)
			return ((void *)(s + i));
		i++;
	}
	return (NULL);
}
