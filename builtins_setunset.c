#include "shell.h"

extern char **environ;

/* count env entries */
static int env_count(void)
{
    int c = 0;

    while (environ && environ[c])
        c++;
    return (c);
}

/* find index of NAME in environ (NAME without '=') */
static int env_find(const char *name)
{
    int i, len;

    if (!name)
        return (-1);

    len = (int)strlen(name);
    for (i = 0; environ && environ[i]; i++)
    {
        if (strncmp(environ[i], name, len) == 0 && environ[i][len] == '=')
            return (i);
    }
    return (-1);
}

int builtin_setenv(char **argv)
{
    int idx, count, i;
    char *new_str;
    char **new_env;

    if (!argv[1] || !argv[2] || argv[3])
    {
        dprintf(2, "Usage: setenv VARIABLE VALUE\n");
        return (1);
    }

    if (argv[1][0] == '\0' || strchr(argv[1], '=') != NULL)
    {
        dprintf(2, "setenv: invalid variable name\n");
        return (1);
    }

    new_str = malloc(strlen(argv[1]) + strlen(argv[2]) + 2);
    if (!new_str)
    {
        dprintf(2, "setenv: malloc failed\n");
        return (1);
    }
    sprintf(new_str, "%s=%s", argv[1], argv[2]);

    idx = env_find(argv[1]);
    if (idx != -1)
    {
        free(environ[idx]);
        environ[idx] = new_str;
        return (0);
    }

    count = env_count();
    new_env = malloc(sizeof(char *) * (count + 2));
    if (!new_env)
    {
        free(new_str);
        dprintf(2, "setenv: malloc failed\n");
        return (1);
    }

    for (i = 0; i < count; i++)
        new_env[i] = environ[i];

    new_env[count] = new_str;
    new_env[count + 1] = NULL;

    /* environ is our malloc'ed copy (created by env_init), safe to free later */
    free(environ);
    environ = new_env;

    return (0);
}

int builtin_unsetenv(char **argv)
{
    int idx, count, i;

    if (!argv[1] || argv[2])
    {
        dprintf(2, "Usage: unsetenv VARIABLE\n");
        return (1);
    }

    idx = env_find(argv[1]);
    if (idx == -1)
        return (0);

    free(environ[idx]);

    count = env_count();
    for (i = idx; i < count; i++)
        environ[i] = environ[i + 1];

    return (0);
}
