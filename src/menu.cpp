#include "menu.h"

menu_t menu;


bool menu_t::load(const char *name, bool test)
{

}

void menu_t::save(const char *name)
{
	_iobuf *h = fopen(name, "wb");

	// fwrite("MENU", 4, 1, h);
	// fwrite(sizeof(menu_t));
	// fwrite(crc32);
	// fwrite(contents);

	fclose(h);
}

int  menu_t::crc32()
{

}