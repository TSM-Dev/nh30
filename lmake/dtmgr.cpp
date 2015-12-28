#include "dtmgr.hpp"

NetworkClass *dtmgr::nwclass;
RecvTable   **dtmgr::nwtable;

extern IClient *client;


void dtmgr::Lineout(RecvTable *table)
{
	static int dtindex = 0;
	nwtable[dtindex++] = table;

	for (int i = 0; i < table->iprops; i++)
	{
		RecvTable *dt = table->props[i].datatable;

		if (dt)
		{
			bool g = true;

			for (int j = 0; j < dtindex && g; j++)
			{
				if (nwtable[j] == dt)
					g = false;
			}

			if (g)
			{
				Lineout(dt);
			}
		}
	}
}

int dtmgr::GetOffset(const char *dt, const char *name)
{
	for (int i = 0;; i++)
	{
		RecvTable *table = nwtable[i];

		if (!table)
			return 0;

		if (*dt == '*' || streq(table->name, dt))
		{
			for (int j = 0; j < table->iprops; j++)
			{
				RecvProp *prop = &table->props[j];

				if (streq(prop->name, name))
				{
					return prop->offset;
				}
			}
		}
	}

	return 0;
}

void dtmgr::SetHook(const char *dt, const char *name, void (*proxyfn)(const RecvProxyData &, void *, RecvProxyResult &))
{
	for (int i = 0;; i++)
	{
		RecvTable *table = nwtable[i];

		if (!table)
			return;

		if (*dt == '*' || streq(table->name, dt))
		{
			for (int j = 0; j < table->iprops; j++)
			{
				RecvProp *prop = &table->props[j];

				if (streq(prop->name, name))
				{
					prop->proxy = (void *)proxyfn;
				}
			}
		}
	}
}

void dtmgr::Start()
{
	nwtable = new RecvTable *[1024];


	for (int i = 0; !nwclass; i++)
	{
		if (*client->GetMethod<char *>(i) == '\xa1')
			nwclass = **(NetworkClass ***)(client->GetMethod<char *>(i) + 1);
	}

	for (NetworkClass *next = nwclass; next; next = next->next)
		Lineout(next->table);

	Map();
}


int m_fFlags;
int m_lifeState;
int m_iHealth;
int m_nSequence;
int m_iTeamNum;
int m_Collision;
int m_angRotation;
#ifdef GMOD
int m_iMaxHealth;
#endif
int m_vecMins;
int m_vecMaxs;
int m_nSolidType;
int m_usSolidFlags;
int m_hActiveWeapon;
int m_iClip1;
int m_flNextPrimaryAttack;
int m_nTickBase;
int m_Local;
#ifdef CSGO
int m_aimPunchAngle;
#else
int m_vecPunchAngle;
#endif
#if defined(L4D) || defined(L4D2)
int m_iMaxHealth;
int m_mobRush;
#endif
int m_vecViewOffset0;

void dtmgr::Map()
{
	m_fFlags = GetOffset("DT_BasePlayer", "m_fFlags");
	m_lifeState = GetOffset("DT_BasePlayer", "m_lifeState");
	m_iHealth = GetOffset("DT_BasePlayer", "m_iHealth");
	m_nSequence = GetOffset("DT_BaseAnimating", "m_nSequence");
	m_iTeamNum = GetOffset("DT_BaseEntity", "m_iTeamNum");
	m_Collision = GetOffset("DT_BaseEntity", "m_Collision");
	m_angRotation = GetOffset("DT_BaseEntity", "m_angRotation");
#ifdef GMOD
	m_iMaxHealth = GetOffset("DT_BaseEntity", "m_iMaxHealth");
#endif
	m_vecMins = GetOffset("DT_CollisionProperty", "m_vecMins");
	m_vecMaxs = GetOffset("DT_CollisionProperty", "m_vecMaxs");
	m_nSolidType = GetOffset("DT_CollisionProperty", "m_nSolidType");
	m_usSolidFlags = GetOffset("DT_CollisionProperty", "m_usSolidFlags");
	m_hActiveWeapon = GetOffset("DT_BaseCombatCharacter", "m_hActiveWeapon");
	m_iClip1 = GetOffset("DT_BaseCombatWeapon", "m_iClip1");
	m_flNextPrimaryAttack = GetOffset("DT_LocalActiveWeaponData", "m_flNextPrimaryAttack");
	m_nTickBase = GetOffset("DT_LocalPlayerExclusive", "m_nTickBase");
	m_Local = GetOffset("DT_LocalPlayerExclusive", "m_Local");
#ifdef CSGO
	m_aimPunchAngle = GetOffset("DT_Local", "m_aimPunchAngle");
#else
	m_vecPunchAngle = GetOffset("DT_Local", "m_vecPunchAngle");
#endif
#if defined(L4D) || defined(L4D2)
	m_iMaxHealth = GetOffset("DT_TerrorPlayer", "m_iMaxHealth");
	m_mobRush = GetOffset("DT_Infected", "m_mobRush");
#endif
	m_vecViewOffset0 = GetOffset("*", "m_vecViewOffset[0]");
}