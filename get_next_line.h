/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_next_line.h                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jlucotte <jlucotte@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/19 00:14:12 by jlucotte          #+#    #+#             */
/*   Updated: 2026/10/21 13:51:42 by jlucotte         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef GET_NEXT_LINE_H
# define GET_NEXT_LINE_H

# ifndef BUFFER_SIZE
#  define BUFFER_SIZE 42
# endif

# include <unistd.h>
# include <stddef.h>
# include <limits.h>
# include <stdlib.h>

# define LINE_END '\n'

char	*get_next_line(int fd);

size_t	ft_strlen(const char *str);
char	*ft_strjoin(char *s1, char *s2, int size1, int size2);
char	*ft_substr(char const *s, unsigned int start, size_t len);
int		is_character_in_str(char *str, char c);

#endif