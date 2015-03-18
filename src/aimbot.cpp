#include "aimbot.h"

extern ITrace *enginetrace;

extern IEngine *engine;
extern IEntities *ents;

extern IMovement *movement;
extern IPrediction *prediction;

extern Globals *globals;

extern int m_fFlags;


int real_flags, next_flags;

void *aimbot::g_movehelper = nullptr;
void  aimbot::RunCommand(UserCmd *ucmd)
{
	if (g_movehelper == nullptr)
		return;

#if defined(VORANGEBOX)
	static int offset = *(int *)util::FindPattern(prediction->GetMethod<void *>(17), 0x100, Q"89 ? .? ? ? ? E8");
#endif
#if defined(VL4D)
	static int offset = *(int *)util::FindPattern(prediction->GetMethod<void *>(18), 0x100, Q"89 ? .? ? ? ? E8");
#endif
#if defined(VALIENSWARM)
	static int offset = *(int *)util::FindPattern(prediction->GetMethod<void *>(19), 0x100, Q"89 ? .? ? ? ? E8");
#endif

	Entity *lp = LocalPlayer();
	WritePtr<UserCmd *>(lp, offset, ucmd);

	GetVFunc<void (__thiscall *)(void *, Entity *)>(g_movehelper, 1)(g_movehelper, lp);


	real_flags = ReadPtr<int>(lp, m_fFlags);

	char movedata[0x100] = {0};

	prediction->SetupMove(lp, ucmd, nullptr, movedata);
	movement->ProcessMovement(lp, movedata);
	prediction->FinishMove(lp, ucmd, movedata);

	next_flags = ReadPtr<int>(lp, m_fFlags);
}

bool aimbot::BulletTrace(Vector v1, Vector v2, BulletFilter *bf)
{
	ray tvec = ray(v1, v2);
	trace tr;

#if defined(TF2)
	enginetrace->TraceRay(tvec, 0x0200400B, bf, tr);
#else
	enginetrace->TraceRay(tvec, 0x46004003, bf, tr);
#endif

	return tr.fraction == 1.0f;
}


bool aimbot::CheckTarget(Entity *lp, Entity *pl)
{
	if (!pl->IsAlive())
		return false;

#if defined(L4D) || defined(L4D2)
	if (pl->GetTeam() == 4)
		return false;
#endif

	// turn into switchtable
	if (lp->GetTeam() == pl->GetTeam())
		return false;

	return true;
}


static int shot, next_shot = 0;

void aimbot::Next()
{
	next_shot = shot;
}

inline float RateOf(UserCmd *ucmd, Entity *lp, Entity *pl, int index)
{
	using namespace aimbot;

	switch (2)
	{
	case 0:
	case 1:

	case 2:
		return next_shot < index ? 0.1f : 1.0f;
	}

	//return lp->GetPos().DistTo(pl->GetPos());


	// angular difference

	//Vector path;
	//VectorAngles(pl->GetPos() - lp->GetPos(), path);

	//return path.DistTo(ucmd->viewangles);
}

bool aimbot::Think(UserCmd *ucmd)
{
	Entity *lp = LocalPlayer();
	Entity *t  = nullptr;

	float best = 3.40282347e+38f;
	

	Vector aim, tp, sp = lp->GetShootPos();
	AngleVectors(ucmd->viewangles, aim);

	BulletFilter bf(lp);

	for (int i = 1, ignore = engine->GetLocalPlayer(), players = globals->maxclients(); i <= ents->GetMaxEntities(); i++)
	{
		if (i == ignore)
			continue;

		if (Entity *pl = ents->GetEntity(i))
		{
			if (pl->IsDormant())
				continue;

			if (strcmp(pl->GetNetworkClass()->name, "Infected") && strcmp(pl->GetNetworkClass()->name, "Witch"))
			{
				if (i > players)
					continue;

				if (!CheckTarget(lp, pl))
					continue;
			}
			else
			{
				if (!pl->NPC_IsAlive())
					continue;
			}

			if (!pl->GetModel())
				continue;

			if (!pl->SetupBones())
				continue;

			float rate = RateOf(ucmd, lp, pl, i);
			if (rate >= best)
				continue;

			bf.against(pl);

			bool next = false;
			int index = 0;

			for (int j = pl->Hitboxes() - 1; j >= 0; j--)
			{
				hitbox *box = pl->GetHitbox(j);

				if (box->group == HITGROUP_HEAD)
				{
					Vector pos = pl->GetBoxPos(box);

					if (BulletTrace(sp, pos, &bf))
					{
						t  = pl;
						tp = pos;

						best = rate;
						shot = i;

						next = true;
					}

					index = j;
					break;
				}
			}

			if (!next && 0) // menu.aimbot.hitscan
			{
				for (int j = 0; j < pl->Hitboxes(); j++)
				{
					if (j == index)
						continue;

					hitbox *box = pl->GetHitbox(j);

					if (box->group && box->group != HITGROUP_HEAD)
					{
						Vector pos = pl->GetBoxPos(box);

						if (BulletTrace(sp, pos, &bf))
						{
							t  = pl;
							tp = pos;

							best = rate;
							shot = i;

							break;
						}
					}
				}
			}
		}
	}

	if (t != nullptr)
	{
		VectorAngles(tp - sp, ucmd->viewangles);

		if (1) // menu.aimbot.autoshot
			ucmd->buttons.add(IN_ATTACK);

		return true;
	}

	return false;
}