/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_next_line_bonus.h                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: luinasci <luinasci@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/12 17:24:30 by luinasci          #+#    #+#             */
/*   Updated: 2024/11/18 16:52:46 by luinasci         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef GET_NEXT_LINE_BONUS_H
# define GET_NEXT_LINE_BONUS_H

# include <stdio.h>
# include <stdlib.h>
# include <unistd.h>
# include <stddef.h>
# include <fcntl.h>
# include <limits.h>

char	*ft_strjoin(char const *s1, char const *s2);
char		*get_next_line(int fd);
char		*ft_strchr(const char *s, int c);
void		*ft_calloc(size_t number, size_t size);
int			ft_strlen(const char *str);
char		*ft_read_file(char *receiver_buffer, int fd);
char		*ft_remainder(char *receiver_buffer);
char		*ft_extract_line(char *receiver_buffer);
int			initialize_buffer(int fd, char **receiver_buffer);

# ifndef BUFFER_SIZE
#  define BUFFER_SIZE 42
# endif

#endif
