#include <stdlib.h>
#include <string.h>

// _Znwj
void * operator new (unsigned int len)
{
	return malloc(len);
}

// _Znaj
void * operator new [] (unsigned int len)
{
	return malloc(len);
}

// _ZdlPv
void operator delete (void *p)
{
	free(p);
}

// _ZdaPv
void operator delete [] (void *p)
{
	free(p);
}



extern "C"
{
	void _pei386_runtime_relocator()
	{
	}

	void _ZTVN10__cxxabiv117__class_type_infoE()
	{
	}


	bool __cxa_guard_acquire(bool &guard)
	{
		return !guard;
	}

	void __cxa_guard_release(bool &guard)
	{
		guard = true;
	}
}