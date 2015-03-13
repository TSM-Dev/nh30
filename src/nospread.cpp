#include "nospread.h"

extern IEngine *engine;
extern IEntities *ents;

void nospread::ApplyRecoil(Entity *pl, Angle &angles, float factor)
{
#if defined(CSS) || defined(CSGO)
	angles += pl->GetViewPunch() * 2.0f * factor;
#endif

#if defined(L4D) || defined(L4D2)
	angles += pl->GetViewPunch() * factor;
#endif
}

void nospread::ApplySpread(int sequence_number, Entity *pl, Angle &angles, float factor)
{
	static auto RandomSeed  = (void  (*)(unsigned int))GetProcAddress(GetModuleHandle("vstdlib"), "RandomSeed" );
	static auto RandomFloat = (float (*)(float, float))GetProcAddress(GetModuleHandle("vstdlib"), "RandomFloat");

	Entity *w = pl->GetActiveWeapon();

	int random_seed = MD5_PseudoRandom(sequence_number) & 0x7fffffff;

#if defined(CSS) || defined(CSGO)

	RandomSeed(random_seed + 1 & 0xff);

	w->UpdateAccuracyPenalty();

	float random1 = RandomFloat(0.0f, M_PI * 2.0f);
	float spread1 = RandomFloat(0.0f, w->GetWeaponCone());
	float random2 = RandomFloat(0.0f, M_PI * 2.0f);
	float spread2 = RandomFloat(0.0f, w->GetWeaponSpread());


	float sin1, cos1, sin2, cos2;
	SinCos(random1, sin1, cos1);
	SinCos(random2, sin2, cos2);


	Vector spread = Vector(1.0f, (cos1 * spread1 + cos2 * spread2) * -factor, (sin1 * spread1 + sin2 * spread2) * factor);
	spread.Normalize();

	Angle shake;
	VectorAngles(spread, shake);

	angles += shake;
#endif

#if defined(L4D) || defined(L4D2)
	static auto SharedRandomFloat = (float (*)(const char *, float, float, int))util::FindSubStart(util::FindString(GetModuleHandle("client"), "SharedRandomFloat"));
 	static int *g_random_seed = *(int **)util::FindPattern((void *)SharedRandomFloat, 0x100, Q"A1 .? ? ? ?");

 	*g_random_seed = random_seed;


	float spread = w->GetWeaponSpread();

	angles.x += SharedRandomFloat("CTerrorGun::FireBullet HorizSpread", -spread, spread, 0) * factor;
	angles.y += SharedRandomFloat("CTerrorGun::FireBullet VertSpread" , -spread, spread, 0) * factor;
#endif
}