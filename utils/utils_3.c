/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils_3.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jcologne <jcologne@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/16 15:01:57 by jcologne          #+#    #+#             */
/*   Updated: 2025/04/16 15:05:15 by jcologne         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/*
** Safe strlen that returns size_t
** @param str String to measure
** @return Length of string
*/
size_t	ft_strlen_size(const char *str)
{
	size_t	len;

	if (str)
		len = ft_strlen(str);
	else
		len = 0;
	return (len);
}

/*
** Joins three strings together
** @param s1 First string
** @param s2 Second string
** @param s3 Third string
** @return New concatenated string
*/
char	*ft_strjoin3(const char *s1, const char *s2, const char *s3)
{
	char	*tmp;
	char	*result;

	tmp = ft_strjoin(s1, s2);
	result = ft_strjoin(tmp, s3);
	free(tmp);
	return (result);
}

/**
 * @brief Appends an element to a NULL-terminated string array.
 * @param array Original array (freed after operation).
 * @param new_element Element to add.
 * @return New array with added element.
 */
char	**ft_array_append(char **array, char *new_element)
{
	int		count;
	char	**new_array;
	int		i;

	count = 0;
	while (array && array[count])
		count++;
	new_array = malloc(sizeof(char *) * (count + 2));
	if (!new_array)
		return (NULL);
	i = 0;
	while (array[i])
	{
		new_array[i] = array[i];
		i++;
	}
	new_array[count] = new_element;
	new_array[count + 1] = NULL;
	free(array);
	return (new_array);
}

/**
 * @brief Frees a t_arg structure and its contents.
 *
 * @param arg_ptr Pointer to a t_arg structure.
 */
void	free_arg(void *arg)
{
	t_arg	*a;

	a = (t_arg *)arg;
	free(a->value);
	free(a);
}

/**
 * @brief Concatenates two strings and frees the first string.
 *
 * @param s1 First string (will be freed). Can be NULL.
 * @param s2 Second string (not freed). Can be NULL.
 * @return char* New string containing `s1` + `s2`. NULL on allocation failure.
 */
char	*ft_strjoin_free(char *s1, const char *s2)
{
	char	*result;

	result = ft_strjoin(s1, s2);
	free(s1);
	return (result);
}
