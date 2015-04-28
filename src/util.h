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
	void *FindProlog(void *);

	__fastcall
	const wchar_t *MakeReadable(const char *, int &);

	inline void *CalcAbsAddress(void *a)
	{
		return (char *)a + *(int *)a + 4;
	}
}