#pragma once
#include <stdio.h>

class menu_t
{
public:
	struct
	{
		bool box;
		bool hp;
		bool weapon;
		bool name;
	}
	esp;


	bool load(const char *, bool = false);
	void save(const char *);
	int  crc32();
};

extern menu_t menu;