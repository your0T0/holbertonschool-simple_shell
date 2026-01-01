#include "shell.h"
#include <unistd.h>

static int match_env(char *env, char *name)
{
	int len = _strlen(name);
	return (_strncmp(env, name, len) == 0 && env[len] == '=');
}

int my_setenv(char *name, char *value)
{
    int i, count;
    char *new_var;
    char **new_env;

    if (!name || !value)
        return (1);

    new_var = malloc(_strlen(name) + _strlen(value) + 2);
    if (!new_var)
        return (1);

    _strcpy(new_var, name);
    _strcat(new_var, "=");
    _strcat(new_var, value);

    for (i = 0; environ && environ[i]; i++)
    {
        if (match_env(environ[i], name))
        {
            free(environ[i]);
            environ[i] = new_var;
            return (0);
        }
    }

    count = i;
    new_env = malloc(sizeof(char *) * (count + 2));
    if (!new_env)
    {
	    free(new_var);
	    return (1);
    }
    for (i = 0; i < count; i++)
        new_env[i] = environ[i];

    new_env[count] = new_var;
    new_env[count + 1] = NULL;
    environ = new_env;

    return (0);
}

int my_unsetenv(char *name)
{
	int i, j;

	if (!name)
		return (1);

	for (i = 0; environ && environ[i]; i++)
	{
		if (match_env(environ[i], name))
		{
			free(environ[i]);
			for (j = i; environ[j]; j++)
				environ[j] = environ[j + 1];
			return (0);
		}
	}
	return (0);
}

