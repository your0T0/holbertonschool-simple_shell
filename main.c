#include <stdio.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include "shell.h"
extern char **environ;
/**
 * main - main function
 *
 * Return: 0
 */
void print_number(unsigned int n)
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
write (2, &buff[i], 1);
}

int main(int ac, char **av)
{
	int inter;
	unsigned long cmd_n;
	int last_status;
	char *line;
	size_t len;
	char *argv[64];
	int i;
	int j;
	int sp;
	pid_t pid;
	FILE *input;
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
	inter = isatty(STDIN_FILENO) && (input == stdin);
	while (1)
{
struct stat st;
int has_slash;
int k;
char *next;
int code;
int n;
int status;
char *tmp;
size_t dlen;
size_t clen;
int idx;
sp = 1;
if (inter)
	write(2, "($) ", 4);
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
k = 0;
while (line[k] && line[k] != '\n')
k++;
line[k] = '\0';
if (line[0] == '\0')
	continue;
for (i = 0; line[i] != '\0'; i++)
{
if (line[i] != ' ')
{
sp = 0;
break;
}
}
if (sp)
	continue;
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
}
argv[i] = NULL;
if (argv[0] == NULL)
	continue;
j = 0;
while (argv[0][j] && "exit"[j] && argv[0][j] == "exit"[j])
j++;
if (argv[0][j] == '\0' && "exit"[j] == '\0')
{
code = last_status;
    if (argv[1] != NULL)
    {
	n = 0;
	idx = 0;
        while (argv[1][idx])
        {
            if (argv[1][idx] < '0' || argv[1][idx] > '9')
            {
                write(2, av[0], _strlen(av[0]));
                write(2, ": ", 2);
                print_number(cmd_n);
                write(2, ": exit: Illegal number: ", 24);
                write(2, argv[1], _strlen(argv[1]));
                write(2, "\n", 1);
                free(line);
                exit(2);
            }
            n = n * 10 + (argv[1][idx] - '0');
            idx++;
        }
        code = n % 256;
    }
    free(line);
    exit(code);
}
if (_strcmp(argv[0], "env") == 0)
{
	k = 0;
    while (environ[k] != NULL)
    {
        write(1, environ[k], _strlen(environ[k]));
        write(1, "\n", 1);
        k++;
    }
    last_status = 0;
    continue;
}
has_slash = 0;
for (i = 0; argv[0][i]; i++)
{
    if (argv[0][i] == '/')
    {
        has_slash = 1;
        break;
    }
}
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

        if (!inter)
            exit(127);
        last_status = 127;
        continue;
    }
    pid = fork();
    if (pid == -1)
    {
        perror("fork");
        last_status = 1;
        continue;
    }
    if (pid == 0)
    {
        execve(argv[0], argv, environ);
        perror("execve");
        exit(1);
    }
    else
    {
        waitpid(pid, &status, 0);
        if (WIFEXITED(status))
            last_status = WEXITSTATUS(status);
        else
            last_status = 1;
    }
    continue;
}
else
{
	char *path = NULL;
	char *path_copy;
	char *dir;
	char *found = NULL;
	path = NULL;
for (j = 0; environ[j] != NULL; j++)
{
    if (_strncmp(environ[j], "PATH=", 5) == 0)
    {
        path = environ[j] + 5;
        break;
    }
}
if (path == NULL || path[0] == '\0')
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
path_copy = _strdup(path);
if (!path_copy)
{
    last_status = 1;
    continue;
}
dir = path_copy;
while (dir && *dir)
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
    if (*dir)
    {
	dlen = _strlen(dir);
	clen = _strlen(argv[0]);
	tmp = malloc(dlen + 1 + clen + 1);
        if (!tmp)
        {
            free(path_copy);
            last_status = 1;
            dir = NULL;
            break;
        }
        _strcpy(tmp, dir);
        _strcat(tmp, "/");
        _strcat(tmp, argv[0]);
	if (stat(tmp, &st) == 0 &&
	S_ISREG(st.st_mode) &&
	access(tmp, X_OK) == 0)
	{
		found = tmp;
		break;
	}
	free(tmp);
	}
	dir = next;
}
free(path_copy);
	if (found != NULL)
	{
	pid = fork();
	if (pid == -1)
	{
	perror("fork");
	last_status = 1;
	free(found);
	continue;
	}
	if (pid == 0)
	{
	execve(found, argv, environ);
	perror("execve");
	exit(1);
	}
	else
	{
	int status;
	waitpid(pid, &status, 0);
	if (WIFEXITED(status))
		last_status = WEXITSTATUS(status);
	else
		last_status = 1;
	}
	free(found);
	continue;
	}
	else
	{
	write(2, av[0], _strlen(av[0]));
	write(2, ": ", 2);
	print_number(cmd_n);
	write(2, ": ", 2);
	write(2, argv[0], _strlen(argv[0]));
	write(2, ": not found\n", 12);
	if (!inter)
		exit(127);
	last_status = 127;
	continue;
	}
}
}
if (input != stdin)
	fclose(input);
free(line);
return (last_status);
}
