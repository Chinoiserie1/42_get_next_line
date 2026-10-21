/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_next_line_utils_bonus.c                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jlucotte <jlucotte@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/20 17:12:47 by jlucotte          #+#    #+#             */
/*   Updated: 2026/10/21 15:36:46 by jlucotte         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "get_next_line_bonus.h"

size_t	ft_strlen(const char *str)
{
	size_t	len;

	len = 0;
	while (str[len] != '\0')
		len++;
	return (len);
}

char	*ft_strjoin(char *s1, char *s2, int size1, int size2)
{
	char	*new_str;
	char	*start;

	if (s1 == NULL || s2 == NULL)
		return (NULL);
	new_str = malloc(size1 + size2 + 1);
	start = new_str;
	if (new_str == NULL)
		return (NULL);
	while (*s1 != '\0')
	{
		*new_str = *s1;
		s1++;
		new_str++;
	}
	while (*s2 != '\0')
	{
		*new_str = *s2;
		new_str++;
		s2++;
	}
	*new_str = '\0';
	free(s1 - size1);
	free(s2 - size2);
	return (start);
}

int	is_character_in_str(char *str, char c)
{
	int	index;

	index = 0;
	while (str[index])
	{
		if (str[index] == c)
			return (index);
		index++;
	}
	return (-1);
}

char	*ft_substr(char const *s, unsigned int start, size_t len)
{
	char	*dup;
	size_t	counter;
	size_t	s_len;

	if (s == NULL)
		return (NULL);
	s_len = ft_strlen(s);
	counter = 0;
	if (start > s_len)
		len = 0;
	else if (len > s_len - start)
		len = s_len - start;
	dup = malloc(len + 1);
	if (dup == NULL)
		return (NULL);
	while (counter < len)
	{
		dup[counter] = s[start + counter];
		counter++;
	}
	dup[counter] = '\0';
	return (dup);
}
