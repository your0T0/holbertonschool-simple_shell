#include <stdio.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include "shell.h"

extern char **environ;

static void print_number(unsigned int n)
{
	char buff[20];
	int i = 0;

	if (n == 0)
	{
		write(2, "0", 1);
		return;
	}
	while (n > 0)
	{
		buff[i++] = (n % 10) + '0';
		n /= 10;
	}
	while (i--)
		write(2, &buff[i], 1);
}

static char *get_path_value(void)
{
	int j;

	for (j = 0; environ && environ[j]; j++)
	{
		if (_strncmp(environ[j], "PATH=", 5) == 0)
			return (environ[j] + 5);
	}
	return (NULL);
}

/* return malloc'd fullpath if found, else NULL */
static char *search_in_path(char *cmd)
{
	char *path, *copy, *dir, *next;
	struct stat st;
	char *tmp;
	size_t dlen, clen;

	path = get_path_value();
	if (!path) /* PATH not set */
		return (NULL);

	copy = _strdup(path);
	if (!copy)
		return (NULL);

	dir = copy;
	while (1)
	{
		/* split by ':' */
		next = dir;
		while (*next && *next != ':')
			next++;

		if (*next == ':')
		{
			*next = '\0';
			next++;
		}
		else
			next = NULL;

		/* EMPTY entry means current directory "." */
		if (dir[0] == '\0')
			dir = ".";

		dlen = _strlen(dir);
		clen = _strlen(cmd);
		tmp = malloc(dlen + 1 + clen + 1);
		if (!tmp)
		{
			free(copy);
			return (NULL);
		}
		_strcpy(tmp, dir);
		_strcat(tmp, "/");
		_strcat(tmp, cmd);

		if (stat(tmp, &st) == 0 && S_ISREG(st.st_mode) && access(tmp, X_OK) == 0)
		{
			free(copy);
			return (tmp); /* found */
		}
		free(tmp);

		if (!next)
			break;
		dir = next;
	}

	free(copy);
	return (NULL);
}

int main(int ac, char **av)
{
	int inter;
	unsigned long cmd_n = 0;
	int last_status = 0;
	char *line = NULL;
	size_t len = 0;
	char *argv[64];
	FILE *input = stdin;

	if (ac == 2)
	{
		input = fopen(av[1], "r");
		if (!input)
		{
			perror(av[0]);
			exit(1);
		}
	}

	inter = isatty(STDIN_FILENO) && (input == stdin);

	while (1)
	{
		struct stat st;
		int i, j, k, sp, has_slash;
		pid_t pid;
		int status;
		char *found = NULL;

		if (inter)
			write(2, "($) ", 4);

		if (my_getline(&line, &len, (input == stdin) ? STDIN_FILENO : fileno(input)) == -1)
			break;

		/* remove trailing newline */
		k = 0;
		while (line[k] && line[k] != '\n')
			k++;
		line[k] = '\0';

		/* skip empty/space-only lines */
		if (line[0] == '\0')
			continue;

		sp = 1;
		for (i = 0; line[i] != '\0'; i++)
		{
			if (line[i] != ' ' && line[i] != '\t')
			{
				sp = 0;
				break;
			}
		}
		if (sp)
			continue;

		/* tokenize */
		cmd_n++;
		i = 0;
		j = 0;
		while (line[j])
		{
			while (line[j] == ' ' || line[j] == '\t')
				j++;
			if (line[j] == '\0')
				break;

			argv[i++] = &line[j];

			while (line[j] && line[j] != ' ' && line[j] != '\t')
				j++;
			if (line[j])
			{
				line[j] = '\0';
				j++;
			}
			if (i >= 63)
				break;
		}
		argv[i] = NULL;

		if (!argv[0])
			continue;

		/* builtin: exit */
		if (_strcmp(argv[0], "exit") == 0)
		{
			free(line);
			line = NULL;
			if (input != stdin)
				fclose(input);
			exit(last_status);
		}

		/* builtin: env */
		if (_strcmp(argv[0], "env") == 0)
		{
			k = 0;
			while (environ && environ[k])
			{
				write(1, environ[k], _strlen(environ[k]));
				write(1, "\n", 1);
				k++;
			}
			last_status = 0;
			continue;
		}

		/* detect slash in command */
		has_slash = 0;
		for (i = 0; argv[0][i]; i++)
		{
			if (argv[0][i] == '/')
			{
				has_slash = 1;
				break;
			}
		}

		/* resolve path */
		if (has_slash)
		{
			if (stat(argv[0], &st) != 0 || !S_ISREG(st.st_mode) || access(argv[0], X_OK) != 0)
			{
				write(2, av[0], _strlen(av[0]));
				write(2, ": ", 2);
				print_number(cmd_n);
				write(2, ": ", 2);
				write(2, argv[0], _strlen(argv[0]));
				write(2, ": not found\n", 12);

				last_status = 127;
				continue;
			}
			found = argv[0];
		}
		else
		{
			found = search_in_path(argv[0]);
			if (!found)
			{
				write(2, av[0], _strlen(av[0]));
				write(2, ": ", 2);
				print_number(cmd_n);
				write(2, ": ", 2);
				write(2, argv[0], _strlen(argv[0]));
				write(2, ": not found\n", 12);

				last_status = 127;
				continue;
			}
		}

		/* fork only after we KNOW it exists */
		pid = fork();
		if (pid == -1)
		{
			perror("fork");
			last_status = 1;
			if (!has_slash)
				free(found);
			continue;
		}

		if (pid == 0)
		{
			execve(found, argv, environ);
			perror("execve");
			exit(1);
		}

		waitpid(pid, &status, 0);
		if (WIFEXITED(status))
			last_status = WEXITSTATUS(status);
		else
			last_status = 1;

		if (!has_slash)
			free(found);
	}

	if (input != stdin)
		fclose(input);
	free(line);
	return (last_status);
}}
