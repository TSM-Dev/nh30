#pragma once
#include "sdk.h"
#include "md5.h"

struct UserCmd;
class  Entity;

namespace nospread
{
	void ApplyRecoil(Entity *pl, Angle &angles, float factor = 1.0f);
	void ApplySpread(int sequence_number, Entity *pl, Angle &angles, float factor = 1.0f);
}