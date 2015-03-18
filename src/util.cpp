#include "util.h"

void *util::FindPattern(void *start, unsigned int len, const pattern *data)
{
	const char *pattern = (char *)data + 5;
	const char *q = (char *)start;

	for (int i = 0, mask = *(int *)((char *)data + 1); i < len; i++, q++)
	{
		const char *seq = pattern;
		const char *mem = q;


		register bool g = true;

		for (int x = 0; x < 32; x++)
		{
			if ((mask & (1 << x)) && (mem[x] != *seq++))
			{
				g = false;
				break;
			}
		}

		if (g) return (void *)(q + (int)*(unsigned char *)data);
	}

	return nullptr;
}

void *util::FindPattern(const char *l, const pattern *data)
{
	return FindPattern(GetModuleHandle(l), -1, data);
}



void *util::FindString(void *p, const char *string)
{
	char *start, *str = nullptr;

	for (start = (char *)p; !str; start++)
	{
		if (strcmp(start, string) == 0)
			str = start;
	}

	for (;; start--)
	{
		if (*(char **)start == str)
			return start;
	}

	return nullptr;
}

const wchar_t *util::MakeReadable(const char *s, int &length)
{
	static wchar_t buf[64];

	wchar_t *p = buf;
	bool u = true;

	for (; *s; s++)
	{
		if (*s == '_')
		{
			if (!u)
			{
				u = true;
				*p++ = ' ';
			}
		}
		else
		{
			if (u)
			{
				u = false;
				*p++ = *s - '\32';
			}
			else
			{
				*p++ = *s;
			}
		}
	}

	*p = '\0';
	length = p - buf;

	return buf;
}