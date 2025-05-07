/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strjoin.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jcologne <jcologne@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/14 16:00:43 by luinasci          #+#    #+#             */
/*   Updated: 2025/05/07 13:41:39 by jcologne         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

char	*ft_strjoin(char const *s1, char const *s2)
{
	size_t	i;
	size_t	j;
	char	*new_str;

	if (!s1 || !s2)
		return (NULL);
	i = 0;
	j = 0;
	new_str = (char *)malloc(ft_strlen(s1) + ft_strlen(s2) + 1);
	if (new_str == NULL)
		return (NULL);
	while (i < (long unsigned int)ft_strlen(s1))
	{
		new_str[i] = s1[i];
		i++;
	}
	while (j < (long unsigned int)ft_strlen(s2))
	{
		new_str[i + j] = s2[j];
		j++;
	}
	new_str[ft_strlen(s1) + ft_strlen(s2)] = '\0';
	return (new_str);
}
