#include "ui.hpp"

extern ISurface *surface;

int ui::font_hud = 0;
int ui::font_ui  = 0;

void ui::Start()
{
	// fonts are created here

	surface->SetFontGlyphSet(font_hud = surface->CreateFont(), "sketchflow print normal", 12, 550, 0, 0, 0x200, 0, 0);
}