/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_next_line_bonus.c                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: luinasci <luinasci@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/12 17:22:52 by luinasci          #+#    #+#             */
/*   Updated: 2024/11/14 17:26:19 by luinasci         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "get_next_line_bonus.h"

char	*get_next_line(int fd)
{
	static char	*receiver_buffer[1024];
	char		*line;

	if (!initialize_buffer(fd, &receiver_buffer[fd]))
		return (NULL);
	receiver_buffer[fd] = ft_read_file(receiver_buffer[fd], fd);
	if (!receiver_buffer[fd])
		return (NULL);
	line = ft_extract_line(receiver_buffer[fd]);
	receiver_buffer[fd] = ft_remainder(receiver_buffer[fd]);
	return (line);
}

char	*ft_read_file(char *receiver_buffer, int fd)
{
	char	*transfer_buffer;
	int		amount_read;
	char	*temp;

	transfer_buffer = (char *)malloc(BUFFER_SIZE + 1);
	if (!transfer_buffer)
		return (NULL);
	amount_read = read (fd, transfer_buffer, BUFFER_SIZE);
	while (amount_read > 0)
	{
		transfer_buffer[amount_read] = '\0';
		temp = ft_strjoin(receiver_buffer, transfer_buffer);
		if (!temp)
			return (free(transfer_buffer), NULL);
		free(receiver_buffer);
		receiver_buffer = temp;
		if (ft_strchr(transfer_buffer, '\n'))
			break ;
		amount_read = read (fd, transfer_buffer, BUFFER_SIZE);
	}
	free(transfer_buffer);
	if (amount_read < 0 || (amount_read == 0
			&& (!receiver_buffer || receiver_buffer[0] == '\0')))
		return (free(receiver_buffer), NULL);
	return (receiver_buffer);
}

char	*ft_extract_line(char *receiver_buffer)
{
	char	*line;
	int		i;

	i = 0;
	if (!receiver_buffer || receiver_buffer[0] == '\0')
		return (NULL);
	while (receiver_buffer[i] != '\0' && receiver_buffer[i] != '\n')
		i++;
	line = (char *)malloc(i + 2);
	if (!line)
		return (NULL);
	i = 0;
	while (receiver_buffer[i] && receiver_buffer[i] != '\n')
	{
		line[i] = receiver_buffer[i];
		i++;
	}
	if (receiver_buffer[i] == '\n')
	{
		line[i] = receiver_buffer[i];
		i++;
	}
	line[i] = '\0';
	return (line);
}

char	*ft_remainder(char *receiver_buffer)
{
	char	*remainder;
	int		i;
	int		j;

	i = 0;
	if (!receiver_buffer)
		return (NULL);
	while (receiver_buffer[i] != '\0' && receiver_buffer[i] != '\n')
		i++;
	if (receiver_buffer[i] == '\0')
		return (free(receiver_buffer), NULL);
	remainder = (char *)malloc(ft_strlen(receiver_buffer) - i + 1);
	if (!remainder)
		return (NULL);
	i++;
	j = 0;
	while (receiver_buffer[i])
		remainder[j++] = receiver_buffer[i++];
	remainder[j] = '\0';
	free(receiver_buffer);
	return (remainder);
}
