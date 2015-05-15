#pragma once
#include <wchar.h>

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

template<typename T>
constexpr T minof()
{
	return (T)1   << (sizeof(T)*8-1);
}

template<typename T>
constexpr T maxof()
{
	return ~((T)1 << (sizeof(T)*8-1));
}

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

inline
const wchar_t *tounicode(const char *text, int &length)
{
	static wchar_t buf[256];
	wchar_t *p = buf;

	for (; *text;)
	{
		register unsigned char t = *(unsigned char *)text++;

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
	*p = L'\0';

	return buf;
}