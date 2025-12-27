#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
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
	int sp;
	char *token;
	pid_t pid;
	(void)ac;
	cmd_n = 0;
	last_status = 0;
	inter = isatty(STDIN_FILENO);
	line = NULL;
	len = 0;
	while (1)
{
sp = 1;
if (inter)
	printf("($) ");
if (getline(&line, &len, stdin) == -1)
	break;
line[strcspn(line, "\n")] = '\0';
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
token = strtok(line, " \t");
i = 0;
while (token != NULL)
{
argv[i++] = token;
token = strtok(NULL, " \t");
}
argv[i] = NULL;
if (strcmp(argv[0], "exit") == 0)
{
	free(line);
	exit(last_status);
}
if (strcmp(argv[0], "env") == 0)
{
	int k = 0;
	while (environ[k] != NULL)
	{
		write(1, environ[k], strlen(environ[k]));
		write(1, "\n", 1);
		k++;
	}
	last_status = 0;
	continue;
}
if (strchr(argv[0], '/') != NULL)
{
	if (access(argv[0], X_OK) != 0)
	{
		write(2, av[0], strlen(av[0]));
		write(2, ": ", 2);
		print_number(cmd_n);
		write(2, ": ",2);
 		write(2, argv[0], strlen(argv[0]));
		write(2, ": not found\n", 12);		
		if (!inter)
			exit(127);
		last_status = 127;
		continue;
	}
	cmd_n++;
	pid = fork();
	if (pid == 0)
	{
	execve(argv[0], argv, environ);
	perror("execve");
	exit(1);
	}
	else if (pid > 0)
	{
	int status;
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
	int j;
	char *path_copy;
	char *dir;
	char *found = NULL;
	char full[1024];
	path = NULL;
for (j = 0; environ[j] != NULL; j++)
{
    if (strncmp(environ[j], "PATH=", 5) == 0)
    {
        path = environ[j] + 5;
        break;
    }
}
if (path == NULL || path[0] == '\0')
{
    write(2, av[0], strlen(av[0]));
    write(2, ": ", 2);
    print_number(cmd_n);
    write(2, ": ", 2);
    write(2, argv[0], strlen(argv[0]));
    write(2, ": not found\n", 12);

    last_status = 127;
    continue;
}
	path_copy = strdup(path);
	dir = strtok(path_copy, ":");
	while (dir != NULL)
{
 if (dir[0] != '\0')
{
snprintf(full, sizeof(full), "%s/%s", dir, argv[0]);
if (access(full, X_OK) == 0)
{
found = strdup(full);
break;
}
}
dir = strtok(NULL, ":");
}
	free(path_copy);
	if (found != NULL)
	{
	cmd_n++;
	pid = fork();
	if (pid == 0)
	{
	execve(found, argv, environ);
	perror("execve");
	exit(1);
	}
	else if (pid > 0)
{
	int status;
	waitpid(pid, &status, 0);
	if (WIFEXITED(status))
	last_status = WEXITSTATUS(status);
	else
	last_status = 1;
}
	free(found);
	}
	else
	{
	write(2, av[0], strlen(av[0]));
	write(2, ": ", 2);
	print_number(cmd_n);
	write(2, ": ", 2);
	write(2, argv[0], strlen(argv[0]));
	write(2, ": not found\n", 12);
	if (!inter)
		exit(127);
	last_status = 127;
continue;
	}
}
}
free(line);
return (last_status);
}
