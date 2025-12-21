#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>

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
char *token;
pid_t pid;
while (1)
{
if (inter)
	printf("($) ");
if (getline(&line, &len, stdin) == -1)
	break;
line[strcspn(line, "\n")] = '\0';
if (line[0] == '\0')
	continue;
token = strtok(line, " ");
i = 0;
while (token != NULL)
{
argv[i++] = token;
token = strtok(NULL, " ");
}
argv[i] = NULL;
pid = fork();
if (pid == 0)
{
execve(argv[0], argv, NULL);
perror("execve");
exit(1);
}
else if (pid > 0)
	wait(NULL);
}
free(line);
return (0);
}
