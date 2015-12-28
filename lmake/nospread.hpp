#pragma once
#include "sdk.hpp"
#include "md5.hpp"

struct UserCmd;
class  Entity;

namespace nospread
{
	void ApplyRecoil(Entity *pl, Angle &angles, float factor = 1.0f);
	void ApplySpread(int sequence_number, Entity *pl, Angle &angles, float factor = 1.0f);
}