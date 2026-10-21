/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_next_line_bonus.h                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jlucotte <jlucotte@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/19 00:14:12 by jlucotte          #+#    #+#             */
/*   Updated: 2026/10/21 15:07:47 by jlucotte         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef GET_NEXT_LINE_BONUS_H
# define GET_NEXT_LINE_BONUS_H

# ifndef BUFFER_SIZE
#  define BUFFER_SIZE 42
# endif

# include <unistd.h>
# include <stddef.h>
# include <limits.h>
# include <stdlib.h>

# define LINE_END '\n'
# define MAX_FD 1024

char	*get_next_line(int fd);

size_t	ft_strlen(const char *str);
char	*ft_strjoin(char *s1, char *s2, int size1, int size2);
char	*ft_substr(char const *s, unsigned int start, size_t len);
int		is_character_in_str(char *str, char c);

#endif