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
int main(int ac, char **av)
{
    int inter;
    unsigned long cmd_n;
    char *line;
    size_t len;
    char *argv[64];
    int i;
    int sp;
    char *token;
    pid_t pid;

    (void)ac;

    cmd_n = 0;
    inter = isatty(STDIN_FILENO);
    line = NULL;
    len = 0;
    i = 0;
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

cmd_n++;
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
    if (access(argv[0], X_OK) != 0)
    {
        dprintf(2, "%s: %lu: %s: not found\n", av[0], cmd_n, argv[0]);
        if (!inter)
            exit(127);
        continue;
    }

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
    char full[1024]; /* <-- هنا مكانها الصح */

    for (j = 0; environ[j] != NULL; j++)
    {
        if (strncmp(environ[j], "PATH=", 5) == 0)
        {
            path = environ[j] + 5;
            break;
        }
    }

    if (path == NULL)
{
    dprintf(2, "%s: %lu: %s: not found\n", av[0], cmd_n, argv[0]);
    if (!inter)
        exit(127);
    continue;
}

if (path[0] == '\0')
{
    char cur[1024];

    snprintf(cur, sizeof(cur), "./%s", argv[0]);
    if (access(cur, X_OK) == 0)
    {
        pid = fork();
        if (pid == 0)
        {
            execve(cur, argv, environ);
            perror("execve");
            exit(1);
        }
        else if (pid > 0)
            wait(NULL);
        continue;
    }

    dprintf(2, "%s: %lu: %s: not found\n", av[0], cmd_n, argv[0]);
    if (!inter)
        exit(127);
    continue;
}

    path_copy = strdup(path);
    dir = strtok(path_copy, ":");

    while (dir != NULL)
    {
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
        free(found);
    }
    else
    {
        dprintf(2, "%s: %lu: %s: not found\n", av[0], cmd_n, argv[0]);
        if (!inter)
            exit(127);
    }
}
}
free(line);
return (0);
}
