#ifndef SHELL_H
#define SHELL_H

/* Standard libraries */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>

/* Function prototypes */
ssize_t my_getline(char **lineptr, size_t *n, int fd);
int _strcmp(char *s1, char *s2);
int _strncmp(char *s1, char *s2, int n);
size_t _strlen(char *s);
char *_strdup(char *s);
char *_strcpy(char *dest, char *src);
char *_strcat(char *dest, char *src);
#endif /* SHELL_H */

