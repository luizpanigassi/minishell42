/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_memcpy.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: luinasci <luinasci@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/10 15:43:44 by luinasci          #+#    #+#             */
/*   Updated: 2024/10/28 17:22:53 by luinasci         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

void	*ft_memcpy(void *dest, const void *src, size_t n)
{
	size_t	i;
	char	*temp_dest;
	char	*str;

	str = (char *)src;
	temp_dest = (char *)dest;
	i = 0;
	if (!dest && !src)
		return (NULL);
	while (i < n)
	{
		temp_dest[i] = str[i];
		i++;
	}
	return (dest);
}
