#include "aimbot.h"

using namespace aimbot;

int
	TargetFilter::next_shot = 0,
	TargetFilter::shot      = 0;

TargetFilter::TargetFilter(UserCmd *a_ucmd, Entity *a_lp)
{
	ucmd = a_ucmd;
	lp   = a_lp;
}

bool TargetFilter::better(Entity *pl, int index)
{

}

void TargetFilter::ack()
{
	
}