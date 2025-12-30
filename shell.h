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

#endif /* SHELL_H */

