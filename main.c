#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>

extern char **environ;

#define PROMPT "($) "
#define MAX_ARGS 64

static int g_interactive = 0;

/* ---------- helpers ---------- */

static void handle_sigint(int sig)
{
	(void)sig;
	if (g_interactive)
		write(STDOUT_FILENO, "\n" PROMPT, sizeof("\n" PROMPT) - 1);
}

static void print_number(unsigned long n)
{
	char buf[32];
	int i = 0;

	if (n == 0)
	{
		write(STDERR_FILENO, "0", 1);
		return;
	}

	while (n > 0 && i < (int)sizeof(buf))
	{
		buf[i++] = (char)('0' + (n % 10));
		n /= 10;
	}
	while (i--)
		write(STDERR_FILENO, &buf[i], 1);
}

static int is_blank_line(const char *s)
{
	size_t i;

	if (!s)
		return (1);

	for (i = 0; s[i]; i++)
	{
		if (s[i] != ' ' && s[i] != '\t' && s[i] != '\n')
			return (0);
	}
	return (1);
}

static void print_env(void)
{
	int i = 0;

	while (environ && environ[i])
	{
		write(STDOUT_FILENO, environ[i], strlen(environ[i]));
		write(STDOUT_FILENO, "\n", 1);
		i++;
	}
}

/* split line into argv in-place (replaces spaces/tabs with '\0') */
static int split_line(char *line, char **argv, int max_args)
{
	int argc = 0;
	char *p = line;

	while (*p)
	{
		while (*p == ' ' || *p == '\t')
			p++;

		if (*p == '\0' || *p == '\n')
			break;

		if (argc >= max_args - 1)
			break;

		argv[argc++] = p;

		while (*p && *p != ' ' && *p != '\t' && *p != '\n')
			p++;

		if (*p == '\0' || *p == '\n')
			break;

		*p = '\0';
		p++;
	}

	argv[argc] = NULL;
	return (argc);
}

static void print_not_found(const char *prog, unsigned long cmd_n, const char *cmd)
{
	write(STDERR_FILENO, prog, strlen(prog));
	write(STDERR_FILENO, ": ", 2);
	print_number(cmd_n);
	write(STDERR_FILENO, ": ", 2);
	write(STDERR_FILENO, cmd, strlen(cmd));
	write(STDERR_FILENO, ": not found\n", 12);
}

/* returns malloc'd full path if found, else NULL */
static char *find_in_path(const char *cmd)
{
	char *path, *copy, *dir, *save;
	size_t dlen, clen;
	char *full;
	struct stat st;

	path = getenv("PATH");
	if (!path || path[0] == '\0')
		return (NULL);

	copy = strdup(path);
	if (!copy)
		return (NULL);

	clen = strlen(cmd);

	dir = strtok_r(copy, ":", &save);
	while (dir)
	{
		if (dir[0] == '\0')
			dir = "."; /* empty entry means current dir */

		dlen = strlen(dir);
		full = malloc(dlen + 1 + clen + 1);
		if (!full)
		{
			free(copy);
			return (NULL);
		}

		memcpy(full, dir, dlen);
		full[dlen] = '/';
		memcpy(full + dlen + 1, cmd, clen);
		full[dlen + 1 + clen] = '\0';

		if (stat(full, &st) == 0 && S_ISREG(st.st_mode) && access(full, X_OK) == 0)
		{
			free(copy);
			return (full);
		}

		free(full);
		dir = strtok_r(NULL, ":", &save);
	}

	free(copy);
	return (NULL);
}

static int execute_cmd(char **argv)
{
	pid_t pid;
	int status;

	pid = fork();
	if (pid == -1)
	{
		perror("fork");
		return (1);
	}

	if (pid == 0)
	{
		execve(argv[0], argv, environ);
		perror("execve");
		_exit(1);
	}

	if (waitpid(pid, &status, 0) == -1)
	{
		perror("waitpid");
		return (1);
	}

	if (WIFEXITED(status))
		return (WEXITSTATUS(status));

	return (1);
}

static int is_number(const char *s)
{
	size_t i;

	if (!s || !*s)
		return (0);

	for (i = 0; s[i]; i++)
	{
		if (s[i] < '0' || s[i] > '9')
			return (0);
	}
	return (1);
}

/* ---------- main ---------- */

int main(int ac, char **av)
{
	FILE *input = stdin;
	char *line = NULL;
	size_t cap = 0;
	ssize_t r;
	unsigned long cmd_n = 0;
	int last_status = 0;
	char *argv[MAX_ARGS];
	int argc;

	if (ac == 2)
	{
		input = fopen(av[1], "r");
		if (!input)
		{
			perror(av[0]);
			return (1);
		}
	}

	g_interactive = isatty(STDIN_FILENO) && (input == stdin);
	signal(SIGINT, handle_sigint);

	while (1)
	{
		if (g_interactive)
			write(STDOUT_FILENO, PROMPT, sizeof(PROMPT) - 1);

		r = getline(&line, &cap, input);
		if (r == -1)
			break;

		cmd_n++;

		if (is_blank_line(line))
			continue;

		argc = split_line(line, argv, MAX_ARGS);
		if (argc == 0 || !argv[0])
			continue;

		/* builtin: exit [status] */
		if (strcmp(argv[0], "exit") == 0)
		{
			int code = last_status;

			if (argv[1] && is_number(argv[1]))
				code = atoi(argv[1]);

			free(line);
			if (input != stdin)
				fclose(input);
			return (code);
		}

		/* builtin: env */
		if (strcmp(argv[0], "env") == 0)
		{
			print_env();
			last_status = 0;
			continue;
		}

		/* execute: if contains '/', run as is */
		if (strchr(argv[0], '/'))
		{
			struct stat st;

			if (stat(argv[0], &st) != 0 || !S_ISREG(st.st_mode) || access(argv[0], X_OK) != 0)
			{
				print_not_found(av[0], cmd_n, argv[0]);
				last_status = 127;
				if (!g_interactive)
				{
					free(line);
					if (input != stdin)
						fclose(input);
					return (127);
				}
				continue;
			}

			last_status = execute_cmd(argv);
			continue;
		}

		/* PATH search (IMPORTANT: no fork if not found) */
		{
			char *full = find_in_path(argv[0]);

			if (!full)
			{
				print_not_found(av[0], cmd_n, argv[0]);
				last_status = 127;
				if (!g_interactive)
				{
					free(line);
					if (input != stdin)
						fclose(input);
					return (127);
				}
				continue;
			}

			argv[0] = full;
			last_status = execute_cmd(argv);
			free(full);
		}
	}

	free(line);
	if (input != stdin)
		fclose(input);

	return (last_status);
}}
