#include "sdk.h"

bool Vector::ToScreen(Vector &screen) const
{
	extern IEngine *engine;

	matrix4x4 &w2s = engine->WorldToScreen();
	float    m3d2d = w2s[3][0] * x + w2s[3][1] * y + w2s[3][2] * z + w2s[3][3];

	if (m3d2d > 0.001f)
	{
		int w, h;
		engine->GetScreenSize(w, h);


		float t = 1.0f / m3d2d;

		screen.x = (w / 2) + (0.5f * ((w2s[0][0] * x + w2s[0][1] * y + w2s[0][2] * z + w2s[0][3]) * t) * w + 0.5f);
		screen.y = (h / 2) - (0.5f * ((w2s[1][0] * x + w2s[1][1] * y + w2s[1][2] * z + w2s[1][3]) * t) * h + 0.5f);

		return true;
	}

	return false;
}


const char *Entity::GetClass()
{
#if defined(VALIENSWARM)
	static auto GetClassnameFn = (const char *(__thiscall *)(void *))util::FindPattern("client", Q"56 8B F1 C6 ? ? ? ? ? ? 8B 46");
#else
	static auto GetClassnameFn = (const char *(__thiscall *)(void *))util::CalcAbsAddress(util::FindPattern("client", Q"E8 .? ? ? ? 50 68 ? ? ? ? FF 15 ? ? ? ? 83 C4 0C 5F 5E 8B E5"));
#endif

	return GetClassnameFn(this);
}

const char *Entity::GetSequence()
{
	extern int m_nSequence;
	static auto GetName =
		(const char *(__thiscall *)(void *, int))util::FindPattern("client", Q"55 8B EC 56 57 8B 7D 08 8B F1 83 FF FF 75 0B");

	return GetName(this, ReadPtr<int>(this, m_nSequence));
}


matrix3x4 matrix[128];
hitboxset *hitboxes;

bool Entity::SetupBones()
{
	extern IModelInfo *mdlinfo;

	static auto BoneSetupFn =
		(bool (__thiscall *)(void *, matrix3x4 *, int, int, float))util::FindSubStart(util::FindString(GetModuleHandle("client"), "*** ERROR: Bone access not allowed (entity %i:%s)\n"));

	void *header = mdlinfo->GetStudiomodel(GetModel());

	if (BoneSetupFn((void **)this + 1, matrix, 128, 0x100, 0.0f))
	{
		if (hitboxes = MakePtr<hitboxset>(header, ReadPtr<int>(header, 0xb0)))
			return true;
	}

	return false;
}

Entity *Entity::GetActiveWeapon()
{
	extern IEntities *ents;
	extern IEngine *engine;

	extern int m_hActiveWeapon;

	return ents->GetEntityByHandle(ReadPtr<void *>(this, m_hActiveWeapon));
}

void Interface::Create(const char *bin, const char *name)
{
	auto CreateInterface = (void *(*)(const char *, void *))GetProcAddress(GetModuleHandle(bin), "CreateInterface");


	char *start = (char *)GetModuleHandle("client");

	int len = strlen(name);
	int nul = strlen(name) + 3;

	for (;; start++)
	{
		if ((memcmp(start, name, len) | *(start + nul)) == 0)
			break;
	}

	thisptr = CreateInterface(start, 0);
	vmthook = new VMT(thisptr);
}

void Interface::Link(void *func, int to)
{
	char *mem = (char *)func;

	DWORD oldprotect;
	VirtualProtect(mem, 10, PAGE_EXECUTE_READWRITE, &oldprotect);

	*(mem+0) = '\xb9';
	*(mem+5) = '\xe9';

	*(void **)(mem+1) = thisptr;
	*(unsigned int *)(mem+6) = 0 - (mem - GetMethod<char *>(to)) - 10;

	VirtualProtect(mem, 10, oldprotect, &oldprotect);
}

