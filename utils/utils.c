/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: luinasci <luinasci@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/26 18:12:41 by luinasci          #+#    #+#             */
/*   Updated: 2025/04/15 17:35:18 by luinasci         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/*
** Frees an array of strings
** @param args Array to free
*/
void ft_free_array(char **args)
{
	int i;

	if (!args)
		return;
	i = -1;
	while (args[++i])
		free(args[i]);
	free(args);
}

/*
** Checks if character is whitespace
** @param c Character to check
** @return 1 if whitespace, 0 otherwise
*/
int ft_isspace(int c)
{
	return (c == ' ' || c == '\t' || c == '\n' || c == '\r' || c == '\f' || c == '\v');
}

/*
** Advances to next character in parser input
** @param p Parser structure
*/
void next_char(t_parse *p)
{
	if (p->input[p->pos])
	{
		p->pos++;
		p->curr_char = p->input[p->pos];
	}
	else
	{
		p->curr_char = '\0';
	}
}

/*
** Checks if character is special shell character
** @param c Character to check
** @return 1 if special, 0 otherwise
*/
int is_special_char(char c)
{
	return (c == '|' || c == '>' || c == '<');
}

/*
** Converts a linked list to string array
** @param lst Linked list to convert
** @return New allocated string array
*/
char **list_to_array(t_list *lst)
{
	char **arr;
	int count;
	int i;
	t_list *tmp;

	count = ft_lstsize(lst);
	arr = malloc((count + 1) * sizeof(char *));
	if (!arr)
		return (NULL);

	i = 0;
	tmp = lst;
	while (tmp)
	{
		arr[i] = ft_strdup((char *)tmp->content);
		if (!arr[i])
		{
			ft_free_array(arr);
			return (NULL);
		}
		tmp = tmp->next;
		i++;
	}
	arr[i] = NULL;
	return (arr);
}

/*
** Frees a command structure
** @param cmd Command to free
*/
void free_cmd(t_cmd *cmd)
{
	t_redir *redir;
	t_redir *tmp;

	if (!cmd)
		return;

	// Free arguments
	ft_free_array(cmd->args);

	// Free redirections
	redir = cmd->redirections;
	while (redir)
	{
		tmp = redir->next;
		free(redir->filename);
		free(redir);
		redir = tmp;
	}
	free(cmd);
}

/*
** String comparison
** @param s1 First string
** @param s2 Second string
** @return Difference between first differing characters
*/
int ft_strcmp(const char *s1, const char *s2)
{
	while (*s1 && *s1 == *s2)
	{
		s1++;
		s2++;
	}
	return (*(unsigned char *)s1 - *(unsigned char *)s2);
}

/*
** Checks if string is numeric
** @param str String to check
** @return 1 if numeric, 0 otherwise
*/
int ft_isnumber(const char *str)
{
	int i;

	if (!str || !*str) // Handle NULL or empty string
		return (0);
	i = 0;
	// Allow optional leading +/-
	if (str[i] == '+' || str[i] == '-')
		i++;
	// Check remaining characters are digits
	while (str[i])
	{
		if (!ft_isdigit(str[i]))
			return (0);
		i++;
	}
	// Must have at least one digit (after optional sign)
	return (i > 0 && (str[0] == '+' || str[0] == '-') ? i > 1 : i > 0);
}

/*
** Checks if string is valid variable name
** @param name String to check
** @return 1 if valid, 0 otherwise
*/
int is_valid_var_name(const char *name)
{
	int i;

	if (!name || !name[0] || ft_isdigit(name[0]))
		return (0);

	i = 0;
	while (name[i])
	{
		if (!ft_isalnum(name[i]) && name[i] != '_')
			return (0);
		i++;
	}
	return (1);
}

/*
** Prints environment variables in export format
*/
void print_export_declarations(void)
{
	extern char **environ;
	int i = 0;

	while (environ[i])
	{
		char *eq = ft_strchr(environ[i], '=');
		if (eq)
		{
			*eq = '\0';
			printf("declare -x %s=\"%s\"\n", environ[i], eq + 1);
			*eq = '=';
		}
		else
			printf("declare -x %s\n", environ[i]);
		i++;
	}
}

/*
** Safe strlen that returns size_t
** @param str String to measure
** @return Length of string
*/
size_t ft_strlen_size(const char *str)
{
	size_t len;
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
char *ft_strjoin3(const char *s1, const char *s2, const char *s3)
{
	char *tmp = ft_strjoin(s1, s2);
	char *result = ft_strjoin(tmp, s3);
	free(tmp);
	return result;
}

/**
 * @brief Appends an element to a NULL-terminated string array.
 * @param array Original array (freed after operation).
 * @param new_element Element to add.
 * @return New array with added element.
 */
char **ft_array_append(char **array, char *new_element)
{
	int count = 0;
	while (array && array[count])
		count++;

	char **new_array = malloc(sizeof(char *) * (count + 2));
	if (!new_array)
		return NULL;

	int i = 0;
	while (array[i])
	{
		new_array[i] = array[i];
		i++;
	}
	new_array[count] = new_element;
	new_array[count + 1] = NULL;

	free(array);
	return new_array;
}

/**
 * @brief Frees a t_arg structure and its contents.
 *
 * @param arg_ptr Pointer to a t_arg structure.
 */
void free_arg(void *arg)
{
	t_arg *a = (t_arg *)arg;
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
char *ft_strjoin_free(char *s1, const char *s2)
{
	char *result = ft_strjoin(s1, s2);
	free(s1);
	return result;
}

/**
 * @brief Safely appends a character to a string.
 * @param str Original string (freed after appending).
 * @param c Character to append.
 * @return New allocated string with appended character.
 */
char *ft_strjoin_char(char *str, char c)
{
	char *new_str;
	size_t len;

	if (!str)
		len = 0;
	else
		len = ft_strlen(str);

	new_str = malloc(len + 2); // Existing chars + new char + null terminator
	if (!new_str)
		return (NULL);

	if (str)
		ft_memcpy(new_str, str, len);
	new_str[len] = c;
	new_str[len + 1] = '\0';

	free(str);
	return (new_str);
}

/**
 * @brief Counts the number of commands in a pipeline.
 *
 * @param pipeline Head of the command pipeline (linked list of t_cmd).
 * @return size_t Number of commands in the pipeline. Returns 0 if pipeline is NULL.
 *
 * @note Traverses the linked list of commands until reaching NULL.
 */
size_t ft_cmd_size(t_cmd *pipeline)
{
	size_t count = 0;
	while (pipeline)
	{
		count++;
		pipeline = pipeline->next;
	}
	return (count);
}

void free_redirections(t_redir *redirs)
{
	t_redir *tmp;
	while (redirs)
	{
		tmp = redirs->next;
		free(redirs->filename);
		free(redirs);
		redirs = tmp;
	}
}

/**
 * @brief Frees a copy of the environment variables.
 * @param env_copy The environment copy to free.
 */
void free_env_copy(char **env_copy)
{
	if (!env_copy)
		return;
	ft_free_array(env_copy); // Reuse your existing ft_free_array function
}

void syntax_error(char *token)
{
	ft_putstr_fd("minishell: syntax error near unexpected token '", STDERR_FILENO);

	if (!token || *token == '\0')
		ft_putstr_fd("newline", STDERR_FILENO);
	else
		ft_putstr_fd(token, STDERR_FILENO);

	ft_putstr_fd("'\n", STDERR_FILENO);
	set_exit_status(SYNTAX_ERROR);
}

char *parse_fd(t_parse *p)
{
	int start = p->pos;
	while (ft_isdigit(p->curr_char))
		next_char(p);
	return ft_substr(p->input, start, p->pos - start);
}
