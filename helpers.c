#include "shell.h"

/**
 * _strlen - returns length of a string
 */
size_t _strlen(char *s)
{
	size_t i = 0;

	if (!s)
		return (0);

	while (s[i])
		i++;

	return (i);
}

/**
 * _strcmp - compares two strings
 * Return: 0 if equal, negative or positive otherwise
 */
int _strcmp(char *s1, char *s2)
{
	int i = 0;

	if (!s1 || !s2)
		return (1);

	while (s1[i] && s2[i] && s1[i] == s2[i])
		i++;
	return (s1[i] - s2[i]);
}

/**
 * _strncmp - compares two strings up to n bytes
 */
int _strncmp(char *s1, char *s2, int n)
{
	int i = 0;
	if (!s1 || !s2)
		return (1);
	while (i < n && s1[i] && s2[i] && s1[i] == s2[i])
		i++;
	if (i == n)
		return (0);
	return (s1[i] - s2[i]);
}

/**
 * _strcpy - copies string src to dest
 */
char *_strcpy(char *dest, char *src)
{
	int i = 0;
	if (!dest || !src)
		return (NULL);
	while (src[i])
	{
		dest[i] = src[i];
		i++;
	}
	dest[i] = '\0';
	return (dest);
}

/**
 * _strcat - concatenates src to dest
 */
char *_strcat(char *dest, char *src)
{
	int i = 0;
	int j = 0;
	if (!dest || !src)
		return (NULL);
	while (dest[i])
		i++;
	while (src[j])
	{
		dest[i + j] = src[j];
		j++;
	}
	dest[i + j] = '\0';
	return (dest);
}

/**
 * _strdup - duplicates a string
 */
char *_strdup(char *s)
{
	char *dup;
	size_t len;
	size_t i;
	if (!s)
		return (NULL);

	len = _strlen(s);
	dup = malloc(len + 1);
	if (!dup)
		return (NULL);
	for (i = 0; i <= len; i++)
		dup[i] = s[i];
	return (dup);
}
