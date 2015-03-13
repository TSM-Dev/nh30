#pragma once
#include <stdlib.h>
#include <string.h>
#include <windows.h>

class pattern;

namespace util
{
	void *FindPattern(void *, unsigned int, const pattern *);
	void *FindPattern(const char *, const pattern *);
	void *FindString(void *, const char *);

	__fastcall
	const wchar_t *MakeReadable(const char *, int &);

	inline void *FindSubStart(void *a)
	{
		for (register char *op = (char *)a;; op--)
		{
			if (*op >> 4 == 0x5 && *(op + 1) == 0x8b && (*(op + 2) & 0xf) == 0xc)
				return op;
		}

		return 0;
	}

	inline void *CalcAbsAddress(void *a)
	{
		return (char *)a + *(int *)a + 4;
	}
}