#pragma once
#include "main.h"

class Entity;

namespace hud
{
	bool MakeBox(Entity *pl, int &x0, int &y0, int &x1, int &y1);

	void DrawESP();
	void DrawCrosshair();
}