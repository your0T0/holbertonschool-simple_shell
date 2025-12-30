#include "shell.h"

#define READ_SIZE 1024

ssize_t my_getline(char **lineptr, size_t *n, int fd)
{
	static char buffer[READ_SIZE];
	static ssize_t buf_len = 0;
	static ssize_t buf_pos = 0;
	static int last_fd = -1;
	ssize_t i = 0;
	if (!lineptr || !n || fd < 0)
		return (-1);
	if (*lineptr == NULL || *n == 0)
	{
		*n = READ_SIZE;
		*lineptr = malloc(*n);
		if (!*lineptr)
			return (-1);
	}
	if (fd != last_fd)
{
        buf_len = 0;
        buf_pos = 0;
        last_fd = fd;
}
	while (1)
	{
		if (buf_pos >= buf_len)
		{
			buf_len = read(fd, buffer, READ_SIZE);
			buf_pos = 0;
			if (buf_len <= 0)
				return (i > 0 ? i : -1);
		}
		if ((size_t)i + 1 >= *n)
{
        size_t new_size = *n * 2;
        char *tmp = malloc(new_size);
        size_t j;

        if (!tmp)
                return (-1);

        for (j = 0; j < (size_t)i; j++)
                tmp[j] = (*lineptr)[j];

        free(*lineptr);
        *lineptr = tmp;
        *n = new_size;
}
		(*lineptr)[i++] = buffer[buf_pos++];
		if ((*lineptr)[i - 1] == '\n')
			break;
	}
	(*lineptr)[i] = '\0';
	return (i);
}
