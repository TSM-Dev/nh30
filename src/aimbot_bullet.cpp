#include "aimbot.hpp"

using namespace aimbot;

bool BulletFilter::ShouldHitEntity(Entity *E, int) const
{
	return (E != lp) && (E != ent);
}

int  BulletFilter::GetTraceType() const
{
	return 0;
}