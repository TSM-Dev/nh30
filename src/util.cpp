#include "util.hpp"

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

void *util::FindProlog(void *ptr)
{
	unsigned char *ins = (unsigned char *)ptr;

	while (ins > 0)
	{
		if ((ins[0]&0xf0) == 0x50 &&
			(ins[1]&0xf0) == 0x80 &&
			(ins[2]&0x0f) == 0x0c)
		{
			return ins;
		}

		ins--;
	}

	return nullptr;
}

void *util::FindString(void *ptr, const char *string)
{
	char *start;
	char *str = nullptr;

	for (start = (char *)ptr; str == nullptr; start++)
	{
		if (streq(start, string))
			str = start;
	}

	while (1)
	{
		if (*(char **)start == str)
			return start;

		start--;
	}

	return nullptr;
}

const wchar_t *util::MakeReadable(const char *s, int &length)
{
	static wchar_t buf[64];

	wchar_t *p = nullptr;
	bool u     = true;

	for (p = buf; *s; s++)
	{
		if (*s == '_')
		{
			if (!u)
			{
				u = true;
				*p++ = L' ';
			}
		}
		else
		{
			if (u)
			{
				u = false;
				*p++ = *s - 32;
			}
			else
			{
				*p++ = *s;
			}
		}
	}

	*p = L'\0';
	length = p - buf;

	return buf;
}