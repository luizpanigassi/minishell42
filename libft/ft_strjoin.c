/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strjoin.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: luinasci <luinasci@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/14 16:00:43 by luinasci          #+#    #+#             */
/*   Updated: 2024/10/29 15:32:50 by luinasci         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

char	*ft_strjoin(char const *s1, char const *s2)
{
	size_t	len1;
	size_t	len2;
	size_t	i;
	size_t	j;
	char	*new_str;

	len1 = ft_strlen(s1);
	len2 = ft_strlen(s2);
	i = 0;
	j = 0;
	new_str = (char *)malloc(len1 + len2 + 1);
	if (new_str == NULL)
		return (NULL);
	while (i < len1)
	{
		new_str[i] = s1[i];
		i++;
	}
	while (j < len2)
	{
		new_str[i + j] = s2[j];
		j++;
	}
	new_str[len1 + len2] = '\0';
	return (new_str);
}
