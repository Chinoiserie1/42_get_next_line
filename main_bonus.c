/* Test program for the get_next_line bonus. Not part of the submission. */

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "get_next_line_bonus.h"

#define MAX_FILES 64

/* Reads one line of fd and prints it. Returns 1 if a line was read, 0 if not. */
static int	print_one_line(int fd, const char *name, int *count)
{
	char	*line;

	line = get_next_line(fd);
	if (line == NULL)
		return (0);
	(*count)++;
	printf("[%s:%d] %s", name, *count, line);
	free(line);
	return (1);
}

/* Reads one line from each fd in turn, until every fd returns NULL. */
static void	read_alternately(int *fds, char **names, int nb_files)
{
	int	counts[MAX_FILES];
	int	active;
	int	i;

	i = 0;
	while (i < nb_files)
		counts[i++] = 0;
	active = 1;
	while (active)
	{
		active = 0;
		i = 0;
		while (i < nb_files)
		{
			if (fds[i] != -1 && print_one_line(fds[i], names[i], &counts[i]))
				active = 1;
			i++;
		}
	}
	i = 0;
	while (i < nb_files)
	{
		printf("[%s] end: %d line(s)\n", names[i], counts[i]);
		i++;
	}
}

/* Checks that invalid fds return NULL without crashing. */
static int	check_invalid_fds(void)
{
	const int	bad_fds[] = {-1, MAX_FD, MAX_FD + 1, 424242};
	const int	nb_bad = sizeof(bad_fds) / sizeof(bad_fds[0]);
	int			status;
	int			i;
	char		*line;

	status = 0;
	i = 0;
	while (i < nb_bad)
	{
		line = get_next_line(bad_fds[i]);
		printf("fd %d -> %s\n", bad_fds[i], line == NULL ? "NULL (OK)" : "KO");
		if (line != NULL)
		{
			free(line);
			status = 1;
		}
		i++;
	}
	return (status);
}

/* Usage: ./gnl_bonus file1 file2 [file3 ...] */
int	main(int argc, char **argv)
{
	int	fds[MAX_FILES];
	int	nb_files;
	int	status;
	int	i;

	printf("BUFFER_SIZE = %d, MAX_FD = %d\n", BUFFER_SIZE, MAX_FD);
	nb_files = argc - 1;
	if (nb_files < 1 || nb_files > MAX_FILES)
	{
		fprintf(stderr, "usage: %s file1 [file2 ...] (max %d)\n",
			argv[0], MAX_FILES);
		return (1);
	}
	status = 0;
	i = 0;
	while (i < nb_files)
	{
		fds[i] = open(argv[i + 1], O_RDONLY);
		if (fds[i] == -1)
		{
			perror(argv[i + 1]);
			status = 1;
		}
		i++;
	}
	read_alternately(fds, argv + 1, nb_files);
	i = 0;
	while (i < nb_files)
	{
		if (fds[i] != -1 && close(fds[i]) == -1)
		{
			perror(argv[i + 1]);
			status = 1;
		}
		i++;
	}
	if (check_invalid_fds() != 0)
		status = 1;
	return (status);
}
