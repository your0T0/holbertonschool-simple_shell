#include "shell.h"

extern char **environ;

/* keep pointer to the env copy we allocate */
static char **g_env_copy = NULL;

/**
 * env_init - prepare a modifiable copy of the environment
 * Return: 0 on success, -1 on failure
 */
int env_init(void)
{
    int i, count = 0;
    char **new_env;

    if (!environ)
        return (0);

    while (environ[count])
        count++;

    new_env = malloc(sizeof(char *) * (count + 1));
    if (!new_env)
        return (-1);

    for (i = 0; i < count; i++)
    {
        new_env[i] = strdup(environ[i]);
        if (!new_env[i])
        {
            while (i > 0)
                free(new_env[--i]);
            free(new_env);
            return (-1);
        }
    }
    new_env[count] = NULL;

    g_env_copy = new_env;
    environ = new_env;

    return (0);
}

/**
 * env_free - free the environment copy
 */
void env_free(void)
{
    int i;

    if (!g_env_copy)
        return;

    for (i = 0; g_env_copy[i]; i++)
        free(g_env_copy[i]);

    free(g_env_copy);
    g_env_copy = NULL;
}}
