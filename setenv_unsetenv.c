#include "shell.h"
#include <unistd.h>
#include <stdlib.h>

int my_setenv(char *name, char *value)
{
    if (!name || !value)
    {
        write(2, "setenv: invalid arguments\n", 26);
        return (1);
    }

    if (setenv(name, value, 1) == -1)
    {
        perror("setenv");
        return (1);
    }

    return (0);
}

int my_unsetenv(char *name)
{
    if (!name)
    {
        write(2, "unsetenv: invalid arguments\n", 28);
        return (1);
    }

    if (unsetenv(name) == -1)
    {
        perror("unsetenv");
        return (1);
    }

    return (0);
}
