#pragma once
#include <stdlib.h>

inline
bool streq(const char *s1, const char *s2)
{
	for (; *s1 == *s2; s1++, s2++)
	{
		if (*s1 == '\0')
			return true;
	}

	return false;
}

inline
bool memeq(const char *m1, const char *m2, int len)
{
	for (; len > 0; len--, m1++, m2++)
	{
		if (*m1 != *m2)
			return false;
	}

	return true;
}

inline
unsigned int qstrlen(const char *str)
{
	register unsigned int len = 0;
	while (*str++)
	{
		len++;
	}

	return len;
}