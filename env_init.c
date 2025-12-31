#include "shell.h"

/**
 * env_init - prepare a modifiable copy of the environment
 * Return: 0 on success, -1 on failure
 */
int env_init(void)
{
    return (0);
}

/**
 * env_free - free the environment copy
 */
void env_free(void)
{
}
int env_init(void)
{
    int i, count = 0;
    char **new_env;

    while (environ[count] != NULL)
        count++;

    new_env = malloc(sizeof(char *) * (count + 1));
    if (new_env == NULL)
        return (-1);

    for (i = 0; i < count; i++)
    {
        new_env[i] = strdup(environ[i]);
        if (new_env[i] == NULL)
        {
            while (i > 0)
                free(new_env[--i]);
            free(new_env);
            return (-1);
        }
    }
    new_env[count] = NULL;
    environ = new_env;
    return (0);
}

void env_free(void)
{
    int i = 0;

    if (environ == NULL)
        return;

    while (environ[i] != NULL)
        free(environ[i++]);

    free(environ);
}
