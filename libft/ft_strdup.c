/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strdup.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: luinasci <luinasci@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/14 15:53:15 by luinasci          #+#    #+#             */
/*   Updated: 2024/10/29 15:32:50 by luinasci         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

char	*ft_strdup(const char *src)
{
	char	*copy;
	size_t	str_length;
	size_t	i;

	i = 0;
	str_length = ft_strlen(src);
	copy = (char *)malloc(str_length + 1);
	if (copy == NULL)
	{
		return (NULL);
	}
	while (i < str_length)
	{
		copy[i] = src[i];
		i++;
	}
	copy[str_length] = '\0';
	return (copy);
}
