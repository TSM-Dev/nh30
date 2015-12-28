#include "aimbot.hpp"

WeaponInfo *aimbot::weaponinfo = 0;

extern ITrace *enginetrace;

extern IEngine *engine;
extern IEntities *ents;

extern IMovement *movement;
extern IPrediction *prediction;

extern Globals *globals;

extern int m_fFlags;


int real_flags, next_flags;

void *aimbot::movehelper = nullptr;
void  aimbot::RunCommand(UserCmd *ucmd)
{
	if (movehelper == nullptr)
		return;

#ifdef VORANGEBOX
	static int offset = *(int *)util::FindPattern(prediction->GetMethod<void *>(17), 0x100, ((const pattern *)"\x02\x41\x00\x00\x00\x89\xE8"));
#endif

#ifdef VL4D
	static int offset = *(int *)util::FindPattern(prediction->GetMethod<void *>(18), 0x100, ((const pattern *)"\x02\x41\x00\x00\x00\x89\xE8"));
#endif

#ifdef VALIENSWARM
	static int offset = *(int *)util::FindPattern(prediction->GetMethod<void *>(19), 0x100, ((const pattern *)"\x02\x41\x00\x00\x00\x89\xE8"));
#endif

	Entity *lp = LocalPlayer();
	WritePtr<UserCmd *>(lp, offset, ucmd);

	GetVFunc<void (__thiscall *)(void *, Entity *)>(movehelper, 1)(movehelper, lp);


	real_flags = ReadPtr<int>(lp, m_fFlags);

	char movedata[0x100] = {0};

	prediction->SetupMove(lp, ucmd, nullptr, movedata);
	movement->ProcessMovement(lp, movedata);
	prediction->FinishMove(lp, ucmd, movedata);

	next_flags = ReadPtr<int>(lp, m_fFlags);
}

bool aimbot::BulletTrace(Vector start, Vector end, BulletFilter *bf)
{
	trace tr;

#ifdef TF2
	enginetrace->TraceRay(ray(start, end), 0x0200400b, bf, tr);
#else
# ifdef WALLBANG
	if (1) // menu.autowall
# endif
	enginetrace->TraceRay(ray(start, end), 0x46004003, bf, tr); // 0x6004003
# ifdef WALLBANG
	else
	{


		//int penetration = weaponinfo->penetration();
		//float damage    = weaponinfo->

		/*
		bool __stdcall L4D_CanPenetrate ( Vector vecStart, Vector vecEnd )
		{
			float flDamage;
			float flLength;
			float flRange;
			float flRangeModifier;
			float flTempLength;


			int iDamage;

			int iPenetration;


			Ray_t ray;
			trace_t gTr;


			trace_t pTr;


			Vector tmpVec;

			Vector vecDir;
			Vector vecSrc;

			vecDir = vecEnd - vecStart;


			flLength = VectorLength ( vecDir );


			vecDir /= flLength;


			g_pAutoWall->m_vecDir = vecDir;

			flDamage = g_pL4DWeaponData->m_fDamage;
			flRange = g_pL4DWeaponData->m_fRange;
			flRangeModifier = g_pL4DWeaponData->m_fRangeModifier;
			iPenetration = g_pL4DWeaponData->m_iPenetration;


			VectorCopy ( vecStart, vecSrc );

			while ( iPenetration )
			{
				iDamage = ( int )flDamage;


				vecEnd = vecSrc + vecDir * flRange;


				pTraceLine ( vecSrc, vecEnd, 0x2004003, g_pBaseClient->pBaseEntity, 0, &gTr );

				if ( gTr.fraction != 1.0f )
				{
					VectorSubtract ( gTr.endpos, vecStart, tmpVec );


					flTempLength = VectorLength ( tmpVec );


					if ( flTempLength >= flLength )
					{
						if ( iDamage >= 1 )
							return true;
					}


					iDamage *= pow ( ( float )flRangeModifier, ( float )( flTempLength * 0.002f ) );


					if ( iPenetration > 1 )
					{
						VectorCopy ( gTr.endpos, g_pAutoWall->m_vecEnd );


						if ( !bGetPointContents() )
							return false;


						pTraceLine ( g_pAutoWall->m_vecSrc, gTr.endpos, 0x200400B, g_pBaseClient->pBaseEntity, 0, &pTr );


						VectorCopy ( pTr.endpos, vecSrc );
					}
				}


				iPenetration--;
			}

			return false;
		}
		*/
	}
# endif
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

float aimbot::RateOf(UserCmd *ucmd, Entity *lp, Entity *pl, int index)
{
	// using namespace aimbot;

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

	weaponinfo = lp->GetActiveWeapon()->GetWeaponInfo();


	Vector aim, tp, sp = lp->GetShootPos();
	AngleVectors(ucmd->viewangles, aim);

	BulletFilter bf(lp);

	for (int i = 1, ignore = engine->GetLocalPlayer(), players = globals->maxclients();
#ifdef NPCS
		i <= ents->GetMaxEntities();
#else
		i <= globals->maxclients();
#endif
		i++)
	{
		if (i == ignore)
			continue;

		if (Entity *pl = ents->GetEntity(i))
		{
			if (pl->IsDormant())
				continue;

			if (i > players)
				continue;

			if (!CheckTarget(lp, pl))
				continue;

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

			if (!next && 1) // menu.aimbot.hitscan
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

		//if (1) // menu.aimbot.autoshot
		//	ucmd->buttons.add(IN_ATTACK);

		return true;
	}

	return false;
}