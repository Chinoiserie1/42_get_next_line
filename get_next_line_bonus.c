/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_next_line_bonus.c                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jlucotte <jlucotte@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/19 00:13:40 by jlucotte          #+#    #+#             */
/*   Updated: 2026/10/21 15:39:16 by jlucotte         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "get_next_line_bonus.h"

char	*reader(int fd, int *total_size_read)
{
	char	*buffer;
	int		size_read;

	size_read = 0;
	buffer = malloc(BUFFER_SIZE + 1);
	if (!buffer)
		return (NULL);
	size_read = read(fd, buffer, BUFFER_SIZE);
	if (size_read == -1)
	{
		free(buffer);
		*total_size_read = -1;
		return (NULL);
	}
	buffer[size_read] = '\0';
	*total_size_read = size_read;
	return (buffer);
}

char	*read_and_check(int fd, char **storage, int *size_read)
{
	char	*buffer;

	buffer = reader(fd, size_read);
	if (*size_read == -1)
	{
		free(*storage);
		return (NULL);
	}
	if (*size_read == 0)
	{
		free(buffer);
		return (*storage);
	}
	*storage = ft_strjoin(*storage, buffer,
			ft_strlen(*storage), ft_strlen(buffer));
	return (*storage);
}

char	*read_loop(int fd, char c, char **storage)
{
	int		size_read;

	size_read = 0;
	*storage = read_and_check(fd, storage, &size_read);
	while (*storage != NULL && is_character_in_str(*storage, c) == -1
		&& size_read > 0)
		*storage = read_and_check(fd, storage, &size_read);
	return (*storage);
}

char	*compute_result(char **storage)
{
	int		index;
	char	*res;
	char	*new_storage;

	index = is_character_in_str(*storage, LINE_END);
	if (index == -1)
	{
		res = ft_substr(*storage, 0, ft_strlen(*storage));
		free(*storage);
		*storage = NULL;
	}
	else
	{
		res = ft_substr(*storage, 0, index + 1);
		if ((int)ft_strlen(*storage) > index)
		{
			new_storage = ft_substr(*storage, index + 1,
					ft_strlen(*storage) - index + 1);
			free(*storage);
			*storage = new_storage;
		}
	}
	return (res);
}

char	*get_next_line(int fd)
{
	static char	*storage[MAX_FD];

	if (BUFFER_SIZE + 1 > SSIZE_MAX || BUFFER_SIZE <= 0
		|| fd < 0 || fd >= MAX_FD)
		return (NULL);
	if (storage[fd] == NULL)
	{
		storage[fd] = malloc(sizeof(char *) * 1);
		if (storage[fd] == NULL)
			return (NULL);
		storage[fd][0] = '\0';
		storage[fd] = read_loop(fd, LINE_END, &storage[fd]);
	}
	else if (storage[fd] != NULL
		&& is_character_in_str(storage[fd], LINE_END) == -1)
		storage[fd] = read_loop(fd, LINE_END, &storage[fd]);
	if (storage[fd] == NULL)
		return (NULL);
	if (ft_strlen(storage[fd]) == 0)
	{
		free(storage[fd]);
		storage[fd] = NULL;
		return (NULL);
	}
	return (compute_result(&storage[fd]));
}
