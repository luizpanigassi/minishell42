/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strrchr.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: luinasci <luinasci@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/09 19:30:11 by luinasci          #+#    #+#             */
/*   Updated: 2024/10/29 15:32:50 by luinasci         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

char	*ft_strrchr(const char *s, int c)
{
	char		ch;
	const char	*found;

	ch = (char)c;
	found = NULL;
	while (*s)
	{
		if (*s == ch)
			found = s;
		s++;
	}
	if (ch == '\0')
		return ((char *)s);
	return ((char *)found);
}
