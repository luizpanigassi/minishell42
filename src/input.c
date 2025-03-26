/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   input.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jcologne <jcologne@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/11 17:44:29 by jcologne          #+#    #+#             */
/*   Updated: 2025/03/17 22:04:15 by jcologne         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void    handle_error(char *message)
{
    perror(message);
    exit(EXIT_FAILURE);
}

char    *get_cmd_path(char *cmd)
{
    char  **paths;
    char  *path;
    int      i;
    paths = ft_split(getenv("PATH"), ':');
    if (!paths)
        handle_error("Error splitting PATH");
    i = 0;
    while (paths[i])
    {
        path = ft_strjoin(paths[i], "/");
        path = ft_strjoin(path, cmd);
        if (access(path, X_OK) == 0)
        {
            free(paths);
            return (path);
        }
        free(path);
        i++;
    }
    free(paths);
    return (NULL);
}