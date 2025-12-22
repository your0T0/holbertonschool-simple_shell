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
int main(void)
{
int inter = isatty(STDIN_FILENO);
char *line = NULL;
size_t len = 0;
char *argv[64];
int i = 0;
int sp;
char *token;
pid_t pid;
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
if (strchr(argv[0], '/') != NULL)
{
pid = fork();
if (pid == 0)
{
execve(argv[0], argv, environ);
perror("execve");
exit(1);
}
else if (pid > 0)
	wait(NULL);
}
else
{
char *path = NULL;
int j;
char *path_copy;
char *dir;
char *found = NULL;
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
	fprintf(stderr, "./hsh: 1: %s: not found\n", argv[0]);
	continue;
}
path_copy = strdup(path);
dir = strtok(path_copy, ":");
while (dir != NULL)
{
char full[1024];
if (dir[0] == '\0')
snprintf(full, sizeof(full), "./%s", argv[0]);
else
snprintf(full, sizeof(full), "%s/%s", dir, argv[0]);
if (access(full, X_OK) == 0)
{
found = strdup(full);
break;
}
dir = strtok(NULL, ":");
}
free(path_copy);
if (found != NULL)
{
pid = fork();
if (pid == 0)
{
execve(found, argv, environ);
perror("execve");
exit(1);
}
else if (pid > 0)
	wait(NULL);
}
else
{
fprintf(stderr, "./hsh: 1: %s: not found\n", argv[0]);
if (!inter)
exit(127);
}
free(found);
}
}
free(line);
return (0);
}
