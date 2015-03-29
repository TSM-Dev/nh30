#include "menu.h"

menu_t menu;


static unsigned int crc32(const void *data, unsigned int len)
{
	unsigned int crc = 0xffffffff;
	unsigned char *p = (unsigned char *)data;

	for (unsigned int i = 0; i < len; i++, p++)
	{
		crc ^= *p;

		for (int j = 7; j >= 0; j--) 
		{
			unsigned int mask = -(crc & 1);

			crc >>= 1;
			crc  ^= 0xedb88320 & mask;
		}
	}

	return ~crc;
}

bool menu_t::load(const char *name, bool test)
{
	// FILE *cfg = 
}

void menu_t::save(const char *name)
{
	// _iobuf *h = fopen(name, "wb");

	// fwrite("MENU", 4, 1, h);
	// fwrite(sizeof(menu_t));
	// fwrite(crc32);
	// fwrite(contents);

	// fclose(h);
}