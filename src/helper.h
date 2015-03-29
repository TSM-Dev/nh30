#pragma once
#include <wchar.h>

#define minof(t) ((t)(((t)1)<<((sizeof(t)*8)-1)))
#define maxof(t) ((t)(((unsigned t)-1)>>1))

#define bpaware() register stack *bp asm("ebp")

struct stack
{
	stack *next;
	char  *ret;

	template<typename T> inline T &arg(unsigned int i)
	{
		return *(T *)((void **)this + i - 1 + 2);
	}
};


template<typename Fn>
inline Fn GetVFunc(const void *v, int i)
{
	return *(*(Fn **)v + i);
}

template<typename T>
inline T ReadPtr(const void *base, int o)
{
	return *(T *)((char *)base + o);
}

template<typename T>
inline T WritePtr(void *base, int o, T v)
{
	return *(T *)((char *)base + o) = v;
}

template<typename T>
inline T *MakePtr(void *base, int o)
{
	return (T *)((char *)base + o);
}


inline const wchar_t *tounicode(const char *text, int &length)
{
	wchar_t buf[64] = {0};
	wchar_t *p = buf;

	for (; *text;)
	{
		register char t = *text++;

		if (t < 0x80)
		{
			*p++ = t;
		}
		else
		{
			*p++ = ((t & 0x1f) << 6) | (*text++ & 0x3f);
		}
	}

	length = p - buf;

	return buf;
}