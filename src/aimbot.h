#pragma once
#include "sdk.h"
#include "vector.h"

struct UserCmd;
struct hitbox;
struct WeaponInfo;
class  Entity;

namespace aimbot
{
	extern void *movehelper;
	extern WeaponInfo *weaponinfo;

	class BulletFilter
	{
		Entity *lp, *ent;

	public:
		BulletFilter(Entity *newlp) : lp(newlp)
		{
		}

		inline void against(Entity *newent)
		{
			ent = newent;
		}


		virtual bool ShouldHitEntity(Entity *ent, int mask) const;
		virtual int  GetTraceType() const;
	};

	void RunCommand(UserCmd *ucmd);

	bool Think(UserCmd *ucmd);

	bool CheckTarget(Entity *lp, Entity *pl);
	bool BulletTrace(Vector v1, Vector v2, BulletFilter *bf);

	float RateOf(UserCmd *ucmd, Entity *lp, Entity *pl, int index);
	void Next();
}