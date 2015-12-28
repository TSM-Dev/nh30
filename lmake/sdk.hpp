#pragma once
#include "vmt.hpp"
#include "util.hpp"
#include "main.hpp"
#include "vector.hpp"
#include "helper.hpp"

#define LocalPlayer() (ents->GetEntity(engine->GetLocalPlayer()))

enum
{
	TEAM_UNASSIGNED,
	TEAM_SPECTATOR,
	TEAM_RED,
	TEAM_BLU,
};

enum
{
	HITGROUP_GENERIC	= 0,
	HITGROUP_HEAD		= 1,
	HITGROUP_CHEST		= 2,
	HITGROUP_STOMACH	= 3,
	HITGROUP_LEFTARM	= 4,
	HITGROUP_RIGHTARM	= 5,
	HITGROUP_LEFTLEG	= 6,
	HITGROUP_RIGHTLEG	= 7,
	HITGROUP_GEAR		= 10,
};

enum
{
	FL_ONGROUND		= 1,
	FL_DUCKING		= (1 << 1),
};

enum
{
	IN_ATTACK		= 1,
	IN_JUMP			= (1 << 1),
	IN_DUCK			= (1 << 2),
	IN_FORWARD		= (1 << 3),
	IN_BACK			= (1 << 4),
	IN_USE			= (1 << 5),
	IN_CANCEL		= (1 << 6),
	IN_LEFT			= (1 << 7),
	IN_RIGHT		= (1 << 8),
	IN_MOVELEFT		= (1 << 9),
	IN_MOVERIGHT	= (1 << 10),
	IN_ATTACK2		= (1 << 11),
	IN_RUN			= (1 << 12),
	IN_RELOAD		= (1 << 13),
	IN_ALT1			= (1 << 14),
	IN_ALT2			= (1 << 15),
	IN_SCORE		= (1 << 16),
	IN_SPEED		= (1 << 17),
	IN_WALK			= (1 << 18),
	IN_ZOOM			= (1 << 19),
	IN_WEAPON1		= (1 << 20),
	IN_WEAPON2		= (1 << 21),
	IN_BULLRUSH		= (1 << 22),
	IN_GRENADE1		= (1 << 23),
	IN_GRENADE2		= (1 << 24),
	IN_ATTACK3		= (1 << 25),
};

struct player_info
{
private:

#ifdef VALIENSWARM
	char	header[0x10];
#endif

#ifdef VL4D
	char	header[0x8];
#endif

public:
	char	name[32];
	int		userid;

private:
	char	unused[0x150];
};

struct hitbox
{
	int		bone;
	int		group;
	Vector	mins;
	Vector	maxs;
	int		nameindex;

private:
	int		unused[8];
};

struct hitboxset
{
	int	nameindex;
	int	numhitboxes;
	int	hitboxindex;
};

struct ray
{
	VectorAligned	start;
	VectorAligned	delta;
	VectorAligned	startoffset;
	VectorAligned	extents;

#ifndef VORANGEBOX
	void	*matrix;
#endif

	bool	isray;
	bool	isswept;

	ray()
	{
	}

	ray(Vector v1, Vector v2)
	{
		delta.x	= v2.x - v1.x;
		delta.y	= v2.y - v1.y;
		delta.z	= v2.z - v1.z;
		start.x	= v1.x;
		start.y	= v1.y;
		start.z	= v1.z;

		isswept	= !delta.IsZero();
		isray	= true;

#ifndef VORANGEBOX
		matrix	= 0;
#endif

		startoffset.Zero();
		extents.Zero();
	}
};

class Entity;
struct trace
{
	Vector		startpos;
	Vector		endpos;

	struct
	{
		Vector	normal;
		float	distance;
		char	type;
		char	signbits;
		short	unused;
	}
	plane;

	float		fraction;
	int			contents;
	short		dispflags;
	bool		allsolid;
	bool		startsolid;
	float		fractionleftsolid;
	const char	*name;
	short		surfaceprops;
	short		flags;
	int			hitgroup;
	short		physicsbone;
	Entity		*ent;
	int			hitbox;
};

struct Globals
{
#ifdef VALIENSWARM
	inline int maxclients() const
	{
		return ReadPtr<int>(this, 0x18);
	}

	inline float interval() const
	{
		return ReadPtr<float>(this, 0x20);
	}
#else
	inline int maxclients() const
	{
		return ReadPtr<int>(this, 0x14);
	}

	inline float interval() const
	{
		return ReadPtr<float>(this, 0x1c);
	}
#endif
};

struct WeaponInfo
{
#ifdef CSS
	/*
	inline int CSS_GetPenetration() const
	{
		return ReadPtr<int>(this, 0x0884);
	}

	inline int CSS_GetDamage() const
	{
		return ReadPtr<int>(this, 0x0888);
	}

	inline float CSS_GetDistance() const
	{
		return ReadPtr<float>(this, 0x088c);
	}

	inline float CSS_GetRangeModifier() const
	{
		return ReadPtr<float>(this, 0x0890);
	}
	*/
#endif

#ifdef L4D2
	inline int penetration() const
	{
		return ReadPtr<int>(this, 0x09c8);
	}

	inline int damage() const
	{
		return ReadPtr<int>(this, 0x09cc);
	}

	inline float distance()
	{
		return ReadPtr<float>(this, 0x09d0);
	}

	inline float range_modifier()
	{
		return ReadPtr<float>(this, 0x09d4);
	}
#endif
};

struct UserCmd
{
private:
	void	*header;

public:
	int		command_number;
	int		tick_count;
	Angle	viewangles;

#ifdef VALIENSWARM
	Vector	aimvector;
#endif

	Vector	move;

	class
	{
		int container;

	public:
		inline int operator=(int i)
		{
			return container = i;
		}
		inline operator int()
		{
			return container;
		}
		inline operator int&()
		{
			return container;
		}

		inline bool test(int i)
		{
			return (container & i) != 0;
		}
		inline void add(int i)
		{
			container |= i;
		}
		inline void del(int i)
		{
			container &= ~i;
		}
	}		buttons;

	char	impulse;
	int		weaponselect;
	int		weaponsubtype;
	int		random_seed;
	short	mousex;
	short	mousey;
	bool	predicted;

#ifdef GMOD
	char 	&mousewheel()
	{
		return *MakePtr<char>(this, 0x59);
	}
#endif
};


class NetworkClass;
class Vector;

class Entity
{
public:
	const char *GetSequence();
	const char *GetClass();
	bool SetupBones();

	inline int Hitboxes()
	{
		extern hitboxset *hitboxes;
		return hitboxes->numhitboxes;
	}

	inline hitbox *GetHitbox(int box)
	{
		extern hitboxset *hitboxes;
		return (hitbox *)((char *)hitboxes + hitboxes->hitboxindex) + box;
	}

	inline Vector GetBoxPos(hitbox *box)
	{
		extern matrix3x4 matrix[128];

#ifdef FIXHEAD
		return Vector
		(
			matrix[box->bone][0][3],
			matrix[box->bone][1][3],
			matrix[box->bone][2][3]
		);
#else
		Vector mins, maxs;

		VectorTransform(box->mins, matrix[box->bone], mins);
		VectorTransform(box->maxs, matrix[box->bone], maxs);

		return (mins + maxs) * 0.5f;
#endif
	}


	Entity *GetActiveWeapon();
	WeaponInfo *GetWeaponInfo();

	inline float & __attribute((pure)) GetNextPrimaryFire() { extern int m_flNextPrimaryAttack; return *(float *)((char *)this + (m_flNextPrimaryAttack)); }


#ifdef CSGO
	inline Angle & __attribute((pure)) GetViewPunch() { extern int m_Local, m_aimPunchAngle; return *(Angle *)((char *)this + (m_Local + m_aimPunchAngle)); }

#else
	inline Angle & __attribute((pure)) GetViewPunch() { extern int m_Local, m_vecPunchAngle; return *(Angle *)((char *)this + (m_Local + m_vecPunchAngle)); }

#endif

	inline int & __attribute((pure)) GetTickCount() { extern int m_nTickBase; return *(int *)((char *)this + (m_nTickBase)); }
	inline int & __attribute((pure)) GetTeam() { extern int m_iTeamNum; return *(int *)((char *)this + (m_iTeamNum)); }
	inline int & __attribute((pure)) GetHealth() { extern int m_iHealth; return *(int *)((char *)this + (m_iHealth)); }

#if defined(GMOD) || defined(L4D) || defined(L4D2)
	inline int & __attribute((pure)) GetMaxHealth() { extern int m_iMaxHealth; return *(int *)((char *)this + (m_iMaxHealth)); }
#else
	inline int GetMaxHealth()
	{
		return 100;
	}
#endif


#ifdef VORANGEBOX
	inline Vector & GetPos() { return GetVFunc<Vector & (__thiscall *)(void *)>((void **)this + 0, 9)((void **)this + 0); }

	inline void * GetModel() { return GetVFunc<void * (__thiscall *)(void *)>((void **)this + 1, 9)((void **)this + 1); }

	inline NetworkClass * GetNetworkClass() { return GetVFunc<NetworkClass * (__thiscall *)(void *)>((void **)this + 2, 2)((void **)this + 2); }
	inline bool IsDormant() { return GetVFunc<bool (__thiscall *)(void *)>((void **)this + 2, 8)((void **)this + 2); }
#endif


#ifdef VL4D
	inline Vector & GetPos() { return GetVFunc<Vector & (__thiscall *)(void *)>((void **)this + 0, 11)((void **)this + 0); }
#endif

#ifdef VALIENSWARM
	inline Vector & GetPos() { return GetVFunc<Vector & (__thiscall *)(void *)>((void **)this + 0, 10)((void **)this + 0); }
#endif

#if defined(VL4D) || defined(VALIENSWARM)
	inline void * GetModel() { return GetVFunc<void * (__thiscall *)(void *)>((void **)this + 1, 8)((void **)this + 1); }

	inline NetworkClass * GetNetworkClass() { return GetVFunc<NetworkClass * (__thiscall *)(void *)>((void **)this + 2, 1)((void **)this + 2); }

# ifdef VALIENSWARM
	inline bool IsDormant()
	{
		return false;
	}
# else
	inline bool IsDormant() { return GetVFunc<bool (__thiscall *)(void *)>((void **)this + 2, 7)((void **)this + 2); }
# endif
#endif

	inline Vector GetShootPos()
	{
		extern int m_vecViewOffset0;
		return GetPos() + ReadPtr<Vector>(this, m_vecViewOffset0);
	}

	inline bool IsAlive()
	{
		extern int m_lifeState;

		if (ReadPtr<bool>(this, m_lifeState))
			return false;

#if defined(L4D) || defined(L4D2)
		if (!NPC_IsAlive())
			return false;
#endif

		return true;
	}


	inline int Clip1()
	{
		extern int m_iClip1;
		return ReadPtr<int>(this, m_iClip1);
	}

#if defined(CSS) || defined(CSGO)
# ifdef CSS
#	define o_spread 371
# endif

# ifdef CSGO
#	define o_spread 478
# endif

	inline float GetWeaponCone() { return GetVFunc<float (__thiscall *)(void *)>((void **)this + 0, o_spread)((void **)this + 0); }
	inline float GetWeaponSpread() { return GetVFunc<float (__thiscall *)(void *)>((void **)this + 0, o_spread+1)((void **)this + 0); }

	inline void UpdateAccuracyPenalty() { return GetVFunc<void (__thiscall *)(void *)>((void **)this + 0, o_spread+2)((void **)this + 0); }
#endif

#if defined(L4D) || defined(L4D2)
	inline float GetWeaponSpread()
	{
# ifdef L4D2
		return ReadPtr<float>(this, 3340);
# endif
	}
#endif

	inline bool NPC_IsAlive()
	{
#ifdef NPCS
		extern int m_Collision;
		extern int m_nSolidType, m_usSolidFlags;

		if (ReadPtr<char>(this, m_Collision + m_nSolidType) == 0)
			return false;

		if ((ReadPtr<short>(this, m_Collision + m_usSolidFlags) & 4) != 0)
			return false;

		if (memeq(GetSequence(), "Death", 5))
			return false;

		return true;
#else
		return false;
#endif
	}

	inline bool HasFlag(int i)
	{
		extern int real_flags;
		return real_flags & i;
	}

	inline bool HasNextFlag(int i)
	{
		extern int next_flags;
		return next_flags & i;
	}
};


class Interface
{
protected:
	void *thisptr;
	VMT  *vmthook;

	void Create(const char *, const char *);
	void Link(void *, int);

public:
	inline void SetHook(int m, void *h, void *org)
	{
		vmthook->hook(m, h, (void **)org);
	}

	template<typename T>
	inline T GetMethod(int i)
	{
		return (T)vmthook->org_vmt[i];
	}
};


class IEntities : public Interface
{
public:
	IEntities()
	{

		Create("client", "VClientEntityList");

		Link((void *)&IEntities::GetEntity, 3);
		Link((void *)&IEntities::GetEntityByHandle, 4);
		Link((void *)&IEntities::GetMaxEntities, 6);
	}


	Entity *GetEntity(int i);
	Entity *GetEntityByHandle(void *h);
	int GetMaxEntities();
};


class IEngineVGui : public Interface
{
public:
	IEngineVGui()
	{

		Create("engine", "VEngineVGui");

		Link((void *)&IEngineVGui::GetPanel, 1);
	}


	unsigned int GetPanel(unsigned int);
};


class ITrace : public Interface
{
public:
	ITrace()
	{

		Create("engine", "EngineTraceClient");

#ifdef VORANGEBOX
		Link((void *)&ITrace::TraceRay, 4);
#endif
#if defined(VALIENSWARM) || defined(VL4D)
		Link((void *)&ITrace::TraceRay, 5);
#endif
	}


#ifdef VORANGEBOX
	void TraceRay(const ray &t, int mask, void *filter, trace &tr);
#endif
#if defined(VALIENSWARM) || defined(VL4D)
	void TraceRay(const ray &t, int mask, void *filter, trace &tr);
#endif
};


class IModelInfo : public Interface
{
public:
	IModelInfo()
	{

		Create("engine", "VModelInfoClient");

#ifdef VORANGEBOX
		Link((void *)&IModelInfo::GetStudiomodel, 28);
#endif
#if defined(VALIENSWARM) || defined(VL4D)
		Link((void *)&IModelInfo::GetStudiomodel, 30);
#endif
	}


#ifdef VORANGEBOX
	void *GetStudiomodel(void *);
#endif
#if defined(VALIENSWARM) || defined(VL4D)
	void *GetStudiomodel(void *);
#endif
};


class ISurface : public Interface
{
public:
	ISurface()
	{

		Create("vguimatsurface", "VGUI_Surface");

#ifdef VORANGEBOX
		Link((void *)&ISurface::SetColor, 11);
		Link((void *)&ISurface::DrawFilledRect, 12);
		Link((void *)&ISurface::DrawOutlinedRect, 14);
		Link((void *)&ISurface::SetFont, 17);
		Link((void *)&ISurface::SetTextColor, 19);
		Link((void *)&ISurface::SetTextPos, 20);
		Link((void *)&ISurface::DrawText, 22);
		Link((void *)&ISurface::CreateFont, 66);
		Link((void *)&ISurface::SetFontGlyphSet, 67);
		Link((void *)&ISurface::GetTextSize, 75);
#endif
#ifdef VL4D
		Link((void *)&ISurface::SetColor, 11);
		Link((void *)&ISurface::DrawFilledRect, 12);
		Link((void *)&ISurface::DrawOutlinedRect, 14);
		Link((void *)&ISurface::SetFont, 17);
		Link((void *)&ISurface::SetTextColor, 19);
		Link((void *)&ISurface::SetTextPos, 20);
		Link((void *)&ISurface::DrawText, 22);
		Link((void *)&ISurface::CreateFont, 63);
		Link((void *)&ISurface::SetFontGlyphSet, 64);
		Link((void *)&ISurface::GetTextSize, 72);
#endif
#ifdef VALIENSWARM
		Link((void *)&ISurface::SetColor, 14);
		Link((void *)&ISurface::DrawFilledRect, 15);
		Link((void *)&ISurface::DrawOutlinedRect, 17);
		Link((void *)&ISurface::SetFont, 22);
		Link((void *)&ISurface::SetTextColor, 24);
		Link((void *)&ISurface::SetTextPos, 25);
		Link((void *)&ISurface::DrawText, 27);
		Link((void *)&ISurface::CreateFont, 70);
		Link((void *)&ISurface::SetFontGlyphSet, 71);
		Link((void *)&ISurface::GetTextSize, 78);
#endif
	}


#ifdef VORANGEBOX
	void SetColor(int r, int g, int b, int a);
	void DrawFilledRect(int x0, int y0, int x1, int y1);
	void DrawOutlinedRect(int x0, int y0, int x1, int y1);
	void SetFont(unsigned long font);
	void SetTextColor(int r, int g, int b, int a);
	void SetTextPos(int x, int y);
	void DrawText(const wchar_t *text, int length, int drawtype);
	unsigned long CreateFont();
	void SetFontGlyphSet(unsigned long font, const char *name, int tall, int weight, int blur, int scanlines, int flags, int mins, int maxs);
	void GetTextSize(unsigned long font, const wchar_t *text, int &w, int &h);
#endif
#ifdef VL4D
	void SetColor(int r, int g, int b, int a);
	void DrawFilledRect(int x0, int y0, int x1, int y1);
	void DrawOutlinedRect(int x0, int y0, int x1, int y1);
	void SetFont(unsigned long font);
	void SetTextColor(int r, int g, int b, int a);
	void SetTextPos(int x, int y);
	void DrawText(const wchar_t *text, int length, int drawtype);
	unsigned long CreateFont();
	void SetFontGlyphSet(unsigned long font, const char *name, int tall, int weight, int blur, int scanlines, int flags, int mins, int maxs);
	void GetTextSize(unsigned long font, const wchar_t *text, int &w, int &h);
#endif
#ifdef VALIENSWARM
	void SetColor(int r, int g, int b, int a);
	void DrawFilledRect(int x0, int y0, int x1, int y1);
	void DrawOutlinedRect(int x0, int y0, int x1, int y1);
	void SetFont(unsigned long font);
	void SetTextColor(int r, int g, int b, int a);
	void SetTextPos(int x, int y);
	void DrawText(const wchar_t *text, int length, int drawtype);
	unsigned long CreateFont();
	void SetFontGlyphSet(unsigned long font, const char *name, int tall, int weight, int blur, int scanlines, int flags, int mins, int maxs);
	void GetTextSize(unsigned long font, const wchar_t *text, int &w, int &h);
#endif
};


class IPrediction : public Interface
{
public:
	IPrediction()
	{

		Create("client", "VClientPrediction");

#ifdef VORANGEBOX
		Link((void *)&IPrediction::SetupMove, 18);
		Link((void *)&IPrediction::FinishMove, 19);
#endif
#ifdef VL4D
		Link((void *)&IPrediction::SetupMove, 19);
		Link((void *)&IPrediction::FinishMove, 20);
#endif
#ifdef VALIENSWARM
		Link((void *)&IPrediction::SetupMove, 20);
		Link((void *)&IPrediction::FinishMove, 21);
#endif
	}


#ifdef VORANGEBOX
	void SetupMove(Entity *pl, UserCmd *ucmd, void *movehelper, void *movedata);
	void FinishMove(Entity *pl, UserCmd *ucmd, void *movedata);
#endif
#ifdef VL4D
	void SetupMove(Entity *pl, UserCmd *ucmd, void *movehelper, void *movedata);
	void FinishMove(Entity *pl, UserCmd *ucmd, void *movedata);
#endif
#ifdef VALIENSWARM
	void SetupMove(Entity *pl, UserCmd *ucmd, void *movehelper, void *movedata);
	void FinishMove(Entity *pl, UserCmd *ucmd, void *movedata);
#endif
};


class IEngine : public Interface
{
public:
	IEngine()
	{

		Create("engine", "VEngineClient");

		Link((void *)&IEngine::GetScreenSize, 5);
		Link((void *)&IEngine::GetPlayerInfo, 8);
		Link((void *)&IEngine::GetLocalPlayer, 12);
		Link((void *)&IEngine::IsInGame, 26);
#ifdef VORANGEBOX
		Link((void *)&IEngine::WorldToScreen, 36);
#endif
#if defined(VALIENSWARM) || defined(VL4D)
		Link((void *)&IEngine::WorldToScreen, 37);
#endif
	}


	void GetScreenSize(int &w, int &h);
	bool GetPlayerInfo(int index, player_info &info);
	int GetLocalPlayer();
	bool IsInGame();
#ifdef VORANGEBOX
	matrix4x4 &WorldToScreen();
#endif
#if defined(VALIENSWARM) || defined(VL4D)
	matrix4x4 &WorldToScreen();
#endif
};


class IMovement : public Interface
{
public:
	IMovement()
	{

		Create("client", "GameMovement");

		Link((void *)&IMovement::ProcessMovement, 1);
	}


	void ProcessMovement(Entity *pl, void *movedata);
};


class IClient : public Interface
{
public:
	IClient()
	{

		Create("client", "VClient");

	}


};
