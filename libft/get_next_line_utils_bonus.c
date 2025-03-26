/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_next_line_utils_bonus.c                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: luinasci <luinasci@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/12 17:24:06 by luinasci          #+#    #+#             */
/*   Updated: 2025/03/24 14:14:20 by luinasci         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "get_next_line_bonus.h"

char	*ft_strchr(const char *s, int c)
{
	char	ch;

	ch = (char)c;
	while (*s)
	{
		if (*s == ch)
			return ((char *)s);
		s++;
	}
	if (ch == '\0')
		return ((char *)s);
	return (((void *)0));
}

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

int	ft_strlen(const char *str)
{
	int	length;

	length = 0;
	while (str[length] != '\0')
	{
		length++;
	}
	return (length);
}

int	initialize_buffer(int fd, char **receiver_buffer)
{
	if (fd < 0 || BUFFER_SIZE <= 0)
	{
		free(*receiver_buffer);
		*receiver_buffer = NULL;
		return (0);
	}
	if (!*receiver_buffer)
		*receiver_buffer = ft_calloc(1, sizeof(char));
	return (1);
}
