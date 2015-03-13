#pragma once
#include "sdk.h"
#include "vector.h"

struct UserCmd;
struct hitbox;
class  Entity;

namespace aimbot
{
	extern void *g_movehelper;

	class BulletFilter
	{
	public:
		Entity *lp, *ent;

		BulletFilter(Entity *a) : lp(a) { };

		inline void against(Entity *a)
		{
			ent = a;
		}


		virtual bool ShouldHitEntity(Entity *, int) const;
		virtual int GetTraceType() const;
	};

	class TargetFilter
	{
	public:
		UserCmd *ucmd;
		Entity  *lp;

		float best;
		float rate;

		static int next_shot;
		static int shot;

		TargetFilter(UserCmd *, Entity *);

		bool better(Entity *, int);
		void ack();

		static void Next();
	};

	void RunCommand(UserCmd *ucmd);
	
	void Next();
	bool Think(UserCmd *ucmd);

	bool CheckTarget(Entity *lp, Entity *pl);
	bool BulletTrace(Vector v1, Vector v2, BulletFilter *bf);
}