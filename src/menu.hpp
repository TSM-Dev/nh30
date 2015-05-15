#pragma once
#include <stdio.h>

class menu_t
{
public:
	struct
	{

	}
	esp;


	bool load(const char *, bool = false);
	void save(const char *);
};

extern menu_t menu;