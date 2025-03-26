/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_memcmp.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: luinasci <luinasci@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/10 16:30:11 by luinasci          #+#    #+#             */
/*   Updated: 2024/10/29 15:32:50 by luinasci         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

int	ft_memcmp(void *ptr1, void *ptr2, size_t n)
{
	unsigned char	*p1;
	unsigned char	*p2;
	size_t			i;

	p1 = (unsigned char *)ptr1;
	p2 = (unsigned char *)ptr2;
	i = 0;
	while (i < n)
	{
		if (p1[i] != p2[i])
		{
			return ((int)(p1[i] - p2[i]));
		}
		i++;
	}
	return (0);
}
