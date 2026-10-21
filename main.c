/* Test program for get_next_line. Not part of the submission. */

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "get_next_line.h"

/* Prints every line of fd, with its number, until get_next_line returns NULL. */
static void	print_all_lines(int fd, const char *name)
{
	char	*line;
	int		count;

	count = 0;
	line = get_next_line(fd);
	while (line != NULL)
	{
		count++;
		printf("[%s:%d] %s", name, count, line);
		free(line);
		line = get_next_line(fd);
	}
	printf("[%s] end: %d line(s)\n", name, count);
}

/* Opens one file, reads it fully, then closes it. Returns 0 on success. */
static int	read_file(const char *path)
{
	int	fd;

	fd = open(path, O_RDONLY);
	if (fd == -1)
	{
		perror(path);
		return (1);
	}
	print_all_lines(fd, path);
	if (close(fd) == -1)
	{
		perror(path);
		return (1);
	}
	return (0);
}

/* Usage: ./gnl file1 [file2 ...]   or   ./gnl < file   (reads stdin) */
int	main(int argc, char **argv)
{
	int	i;
	int	status;

	printf("BUFFER_SIZE = %d\n", BUFFER_SIZE);
	if (argc < 2)
	{
		print_all_lines(STDIN_FILENO, "stdin");
		return (0);
	}
	status = 0;
	i = 1;
	while (i < argc)
	{
		if (read_file(argv[i]) != 0)
			status = 1;
		i++;
	}
	return (status);
}
