#include <signal.h>
#include <stdio.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include "shell.h"

extern char **environ;

/* ---------- helpers (printing) ---------- */

static void handle_sigint(int sig)
{
	(void)sig;
	write(STDOUT_FILENO, "\n($) ", 5);
}

static void print_number(unsigned long n)
{
	char buff[32];
	int i;

	if (n == 0)
	{
		write(2, "0", 1);
		return;
	}

	i = 0;
	while (n > 0)
	{
		buff[i++] = (n % 10) + '0';
		n /= 10;
	}

	while (i--)
		write(2, &buff[i], 1);
}

static void print_not_found(char *prog, unsigned long cmd_n, char *cmd)
{
	write(2, prog, _strlen(prog));
	write(2, ": ", 2);
	print_number(cmd_n);
	write(2, ": ", 2);
	write(2, cmd, _strlen(cmd));
	write(2, ": not found\n", 12);
}

static int is_number(char *s)
{
	int i;

	if (s == NULL || s[0] == '\0')
		return (0);

	i = 0;
	while (s[i])
	{
		if (s[i] < '0' || s[i] > '9')
			return (0);
		i++;
	}
	return (1);
}

/* ---------- command lookup ---------- */

static int has_slash(char *s)
{
	int i;

	if (s == NULL)
		return (0);

	for (i = 0; s[i]; i++)
	{
		if (s[i] == '/')
			return (1);
	}
	return (0);
}

static char *get_path_value(void)
{
	int i;

	if (environ == NULL)
		return (NULL);

	for (i = 0; environ[i] != NULL; i++)
	{
		if (_strncmp(environ[i], "PATH=", 5) == 0)
			return (environ[i] + 5);
	}
	return (NULL);
}

static char *build_candidate(char *dir, char *cmd)
{
	size_t dlen, clen;
	char *tmp;

	/* empty entry in PATH means current directory */
	if (dir == NULL || dir[0] == '\0')
		dir = ".";

	dlen = _strlen(dir);
	clen = _strlen(cmd);

	tmp = malloc(dlen + 1 + clen + 1);
	if (tmp == NULL)
		return (NULL);

	_strcpy(tmp, dir);
	_strcat(tmp, "/");
	_strcat(tmp, cmd);

	return (tmp);
}

/*
 * Returns malloc'd full path if executable exists, else NULL.
 * IMPORTANT: this ensures we do NOT fork if command doesn't exist.
 */
static char *resolve_command(char *cmd)
{
	struct stat st;
	char *path, *copy, *dir, *next, *candidate;

	if (cmd == NULL || cmd[0] == '\0')
		return (NULL);

	/* If cmd contains '/', check directly */
	if (has_slash(cmd))
	{
		if (stat(cmd, &st) == 0 && S_ISREG(st.st_mode) && access(cmd, X_OK) == 0)
			return (_strdup(cmd));
		return (NULL);
	}

	/* No slash: search in PATH */
	path = get_path_value();
	if (path == NULL)
		return (NULL);

	copy = _strdup(path);
	if (copy == NULL)
		return (NULL);

	dir = copy;
	while (dir)
	{
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

		candidate = build_candidate(dir, cmd);
		if (candidate == NULL)
		{
			free(copy);
			return (NULL);
		}

		if (stat(candidate, &st) == 0 && S_ISREG(st.st_mode) && access(candidate, X_OK) == 0)
		{
			free(copy);
			return (candidate); /* keep candidate */
		}

		free(candidate);
		dir = next;
	}

	free(copy);
	return (NULL);
}

/* ---------- parsing ---------- */

static int only_spaces(char *s)
{
	int i;

	if (s == NULL)
		return (1);

	for (i = 0; s[i]; i++)
	{
		if (s[i] != ' ' && s[i] != '\t' && s[i] != '\n')
			return (0);
	}
	return (1);
}

static int split_line(char *line, char **argv, int max)
{
	int i, j;

	i = 0;
	j = 0;

	while (line[j] && i < (max - 1))
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
	}

	argv[i] = NULL;
	return (i);
}

/* ---------- main ---------- */

int main(int ac, char **av)
{
	int inter;
	unsigned long cmd_n;
	int last_status;
	char *line;
	size_t len;
	char *argv[64];
	pid_t pid;
	FILE *input;

	signal(SIGINT, handle_sigint);

	cmd_n = 0;
	last_status = 0;
	line = NULL;
	len = 0;

	input = stdin;
	if (ac == 2)
	{
		input = fopen(av[1], "r");
		if (!input)
		{
			perror(av[0]);
			exit(1);
		}
	}

	inter = (ac == 1 && isatty(STDIN_FILENO));

	while (1)
	{
		int k, argc;
		int status;
		char *cmd_path;

		if (inter)
			write(STDOUT_FILENO, "($) ", 4);

		if (ac == 2)
		{
			if (my_getline(&line, &len, fileno(input)) == -1)
				break;
		}
		else
		{
			if (my_getline(&line, &len, STDIN_FILENO) == -1)
				break;
		}

		/* trim newline */
		k = 0;
		while (line[k] && line[k] != '\n')
			k++;
		line[k] = '\0';

		if (line[0] == '\0' || only_spaces(line))
			continue;

		cmd_n++;

		argc = split_line(line, argv, 64);
		if (argc == 0 || argv[0] == NULL)
			continue;

		/* builtins */
		if (strcmp(argv[0], "exit") == 0)
		{
			int code;

			if (argv[1] == NULL)
			{
				if (input != stdin)
					fclose(input);
				free(line);
				exit(last_status);
			}

			if (!is_number(argv[1]))
			{
				write(2, av[0], _strlen(av[0]));
				write(2, ": ", 2);
				print_number(cmd_n);
				write(2, ": exit: Illegal number: ", 24);
				write(2, argv[1], _strlen(argv[1]));
				write(2, "\n", 1);

				if (input != stdin)
					fclose(input);
				free(line);
				exit(2);
			}

			code = atoi(argv[1]) % 256;
			if (input != stdin)
				fclose(input);
			free(line);
			exit(code);
		}

		if (_strcmp(argv[0], "env") == 0)
		{
			int e;

			if (environ != NULL)
			{
				for (e = 0; environ[e] != NULL; e++)
				{
					write(1, environ[e], _strlen(environ[e]));
					write(1, "\n", 1);
				}
			}
			last_status = 0;
			continue;
		}

		if (strcmp(argv[0], "setenv") == 0)
		{
			if (argv[1] && argv[2] && !argv[3])
				setenv(argv[1], argv[2], 1);
			print_env();
			last_status = 0;
			continue;
		}

		if (strcmp(argv[0], "unsetenv") == 0)
		{
			if (argv[1] && !argv[2])
				unsetenv(argv[1]);
			print_env();
			last_status = 0;
			continue;
		}

		/* resolve first, so we never fork if not found */
		cmd_path = resolve_command(argv[0]);
		if (cmd_path == NULL)
		{
			print_not_found(av[0], cmd_n, argv[0]);
			last_status = 127;

			if (!inter)
			{
				if (input != stdin)
					fclose(input);
				free(line);
				exit(127);
			}
			continue;
		}

		pid = fork();
		if (pid == -1)
		{
			perror("fork");
			free(cmd_path);
			last_status = 1;
			continue;
		}

		if (pid == 0)
		{
			execve(cmd_path, argv, environ);
			perror("execve");
			exit(1);
		}

		waitpid(pid, &status, 0);
		if (WIFEXITED(status))
			last_status = WEXITSTATUS(status);
		else
			last_status = 1;

		free(cmd_path);
	}

	if (input != stdin)
		fclose(input);
	free(line);
	return (last_status);
}
