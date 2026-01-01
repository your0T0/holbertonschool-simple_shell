#ifndef SHELL_H
#define SHELL_H

/* Standard libraries */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <signal.h>

/* some systems need this for dprintf */
#include <stdarg.h>

extern char **environ;
extern int env_dynamic;
/* env copy init/free */
int env_init(void);
void env_free(void);

/* builtins */
int builtin_setenv(char **argv);
int builtin_unsetenv(char **argv);

/* Function prototypes */
ssize_t my_getline(char **lineptr, size_t *n, int fd);
int my_setenv(char *name, char *value);
int my_unsetenv(char *name);
int _strcmp(char *s1, char *s2);
int _strncmp(char *s1, char *s2, int n);
size_t _strlen(char *s);
char *_strdup(char *s);
char *_strcpy(char *dest, char *src);
char *_strcat(char *dest, char *src);

#endif /* SHELL_H */
