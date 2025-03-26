/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_calloc.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: luinasci <luinasci@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/14 15:42:17 by luinasci          #+#    #+#             */
/*   Updated: 2024/10/29 15:32:50 by luinasci         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

void	*ft_calloc(size_t number, size_t size)
{
	size_t	total_size;
	size_t	i;
	void	*ptr;

	total_size = number * size;
	i = 0;
	ptr = malloc(total_size);
	if (!ptr)
	{
		return (NULL);
	}
	while (i < total_size)
	{
		((unsigned char *)ptr)[i] = 0;
		i++;
	}
	return (ptr);
}
