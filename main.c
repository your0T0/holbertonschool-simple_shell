#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

int main(void)
{
int inter = isatty(STDIN_FILENO);
char *line = NULL;
size_t len = 0;
char *argv[64];
int i = 0;
char *token = strtok(line, " ");
while (1)
{
if (inter)
	printf("($) ");
if (getline(&line, &len, stdin) == -1)
	break;
line[strcspn(line, "\n")] = '\0';
if (line[0] == '\0')
	continue;
while (token != NULL)
{
argv[i++] = token;
token = strtok(NULL, " ");
}
argv[i] = NULL;
}
free(line);
return (0);
}
