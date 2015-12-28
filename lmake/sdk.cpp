#include "sdk.hpp"

bool Vector::ToScreen(Vector &screen) const
{
	extern IEngine *engine;

	matrix4x4 &w2s = engine->WorldToScreen();
	float    m3d2d = w2s[3][0] * x + w2s[3][1] * y + w2s[3][2] * z + w2s[3][3];

	if (m3d2d > 0.001f)
	{
		int w = 1, h = 1;
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
#ifdef VALIENSWARM
	static auto GetClassnameFn = (const char *(__thiscall *)(void *))util::FindPattern("client", ((const pattern *)"\x00\x0F\x0C\x00\x00\x56\x8B\xF1\xC6\x8B\x46"));
#endif

#if defined(VORANGEBOX) || defined(VL4D)
	static auto GetClassnameFn =
		(const char *(__thiscall *)(void *))util::CalcAbsAddress(util::FindPattern("client", ((const pattern *)"\x01\x61\x18\xFE\x00\xE8\x50\x68\xFF\x15\x83\xC4\x0C\x5F\x5E\x8B\xE5")));
#endif

	return GetClassnameFn(this);
}

const char *Entity::GetSequence()
{
	extern int m_nSequence;
	static auto GetName =
		(const char *(__thiscall *)(void *, int))util::FindPattern("client", ((const pattern *)"\x00\xFF\x7F\x00\x00\x55\x8B\xEC\x56\x57\x8B\x7D\x08\x8B\xF1\x83\xFF\xFF\x75\x0B"));

	return GetName(this, ReadPtr<int>(this, m_nSequence));
}


matrix3x4 matrix[128];
hitboxset *hitboxes;

bool Entity::SetupBones()
{
	extern IModelInfo *mdlinfo;

	static auto BoneSetupFn =
		(bool (__thiscall *)(void *, matrix3x4 *, int, int, float))util::FindProlog(util::FindString(GetModuleHandle("client"), "*** ERROR: Bone access not allowed (entity %i:%s)\n"));

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

WeaponInfo *Entity::GetWeaponInfo()
{
#if defined(L4D) || defined(L4D2) || defined(CSS)
	static auto LookupWeaponInfoSlot =
		(unsigned short (*)(const char *))util::FindPattern("client", ((const pattern *)"\x00\xFF\x1F\x00\x00\x55\x8B\xEC\x8B\x45\x08\x83\xEC\x08\x85\xC0\x74\x18"));
	static auto GetFileWeaponInfoFromHandle =
		(WeaponInfo *(*)(unsigned short))util::FindPattern("client", ((const pattern *)"\x00\xFF\x03\x00\x00\x55\x8B\xEC\x66\x8B\x45\x08\x66\x3B\x05"));

	return GetFileWeaponInfoFromHandle(LookupWeaponInfoSlot(GetClass()));
#endif

#if defined(CSGO)
	static auto LookupWeaponInfoSlot =
		(unsigned short (*)(const char *))util::FindPattern("client", ((const pattern *)"\x00\xFF\x00\x00\x00\x55\x8B\xEC\x56\x57\x8B\xF9\xB8"));
	static auto GetFileWeaponInfoFromHandle =
		(WeaponInfo *(*)(unsigned short))util::FindPattern("client", ((const pattern *)"\x00\x87\x01\x00\x00\x66\x3B\x0D\x72\x2E"));
#endif

	return nullptr;
}

void Interface::Create(const char *module, const char *name)
{
	auto CreateInterface = (void *(*)(const char *, void *))GetProcAddress(GetModuleHandle(module), "CreateInterface");


	char *start = (char *)GetModuleHandle("client");

	int len = qstrlen(name);
	int nul = qstrlen(name) + 3;

	for (;; start++)
	{
		if (memeq(start, name, len) && *(start + nul) == '\0')
			break;
	}

	thisptr = CreateInterface(start, 0);
	vmthook = new VMT(thisptr);
}

void Interface::Link(void *func, int index)
{
	DWORD oldprotect;
	VirtualProtect(func, 10, PAGE_EXECUTE_READWRITE, &oldprotect);

	*((char *)func + 0) = '\xb9';
	*((char *)func + 5) = '\xe9';

	*(void **)((char *)func + 1) = thisptr;
	*(int *)((char *)func + 6)   = (int)(GetMethod<char *>(index) - ((char *)func + 10));
	
	VirtualProtect(func, 10, oldprotect, &oldprotect);
}


Entity * __attribute((noinline)) IEntities::GetEntity(int i)
{
	asm("");
}

Entity * __attribute((noinline)) IEntities::GetEntityByHandle(void *h)
{
	asm("");
}

int __attribute((noinline)) IEntities::GetMaxEntities()
{
	asm("");
}

unsigned int __attribute((noinline)) IEngineVGui::GetPanel(unsigned int)
{
	asm("");
}

#ifdef VORANGEBOX
void __attribute((noinline)) ITrace::TraceRay(const ray &t, int mask, void *filter, trace &tr)
{
	asm("");
}

#endif
#if defined(VALIENSWARM) || defined(VL4D)
void __attribute((noinline)) ITrace::TraceRay(const ray &t, int mask, void *filter, trace &tr)
{
	asm("");
}

#endif
#ifdef VORANGEBOX
void * __attribute((noinline)) IModelInfo::GetStudiomodel(void *)
{
	asm("");
}

#endif
#if defined(VALIENSWARM) || defined(VL4D)
void * __attribute((noinline)) IModelInfo::GetStudiomodel(void *)
{
	asm("");
}

#endif
#ifdef VORANGEBOX
void __attribute((noinline)) ISurface::SetColor(int r, int g, int b, int a)
{
	asm("");
}

void __attribute((noinline)) ISurface::DrawFilledRect(int x0, int y0, int x1, int y1)
{
	asm("");
}

void __attribute((noinline)) ISurface::DrawOutlinedRect(int x0, int y0, int x1, int y1)
{
	asm("");
}

void __attribute((noinline)) ISurface::SetFont(unsigned long font)
{
	asm("");
}

void __attribute((noinline)) ISurface::SetTextColor(int r, int g, int b, int a)
{
	asm("");
}

void __attribute((noinline)) ISurface::SetTextPos(int x, int y)
{
	asm("");
}

void __attribute((noinline)) ISurface::DrawText(const wchar_t *text, int length, int drawtype)
{
	asm("");
}

unsigned long __attribute((noinline)) ISurface::CreateFont()
{
	asm("");
}

void __attribute((noinline)) ISurface::SetFontGlyphSet(unsigned long font, const char *name, int tall, int weight, int blur, int scanlines, int flags, int mins, int maxs)
{
	asm("");
}

void __attribute((noinline)) ISurface::GetTextSize(unsigned long font, const wchar_t *text, int &w, int &h)
{
	asm("");
}

#endif
#ifdef VL4D
void __attribute((noinline)) ISurface::SetColor(int r, int g, int b, int a)
{
	asm("");
}

void __attribute((noinline)) ISurface::DrawFilledRect(int x0, int y0, int x1, int y1)
{
	asm("");
}

void __attribute((noinline)) ISurface::DrawOutlinedRect(int x0, int y0, int x1, int y1)
{
	asm("");
}

void __attribute((noinline)) ISurface::SetFont(unsigned long font)
{
	asm("");
}

void __attribute((noinline)) ISurface::SetTextColor(int r, int g, int b, int a)
{
	asm("");
}

void __attribute((noinline)) ISurface::SetTextPos(int x, int y)
{
	asm("");
}

void __attribute((noinline)) ISurface::DrawText(const wchar_t *text, int length, int drawtype)
{
	asm("");
}

unsigned long __attribute((noinline)) ISurface::CreateFont()
{
	asm("");
}

void __attribute((noinline)) ISurface::SetFontGlyphSet(unsigned long font, const char *name, int tall, int weight, int blur, int scanlines, int flags, int mins, int maxs)
{
	asm("");
}

void __attribute((noinline)) ISurface::GetTextSize(unsigned long font, const wchar_t *text, int &w, int &h)
{
	asm("");
}

#endif
#ifdef VALIENSWARM
void __attribute((noinline)) ISurface::SetColor(int r, int g, int b, int a)
{
	asm("");
}

void __attribute((noinline)) ISurface::DrawFilledRect(int x0, int y0, int x1, int y1)
{
	asm("");
}

void __attribute((noinline)) ISurface::DrawOutlinedRect(int x0, int y0, int x1, int y1)
{
	asm("");
}

void __attribute((noinline)) ISurface::SetFont(unsigned long font)
{
	asm("");
}

void __attribute((noinline)) ISurface::SetTextColor(int r, int g, int b, int a)
{
	asm("");
}

void __attribute((noinline)) ISurface::SetTextPos(int x, int y)
{
	asm("");
}

void __attribute((noinline)) ISurface::DrawText(const wchar_t *text, int length, int drawtype)
{
	asm("");
}

unsigned long __attribute((noinline)) ISurface::CreateFont()
{
	asm("");
}

void __attribute((noinline)) ISurface::SetFontGlyphSet(unsigned long font, const char *name, int tall, int weight, int blur, int scanlines, int flags, int mins, int maxs)
{
	asm("");
}

void __attribute((noinline)) ISurface::GetTextSize(unsigned long font, const wchar_t *text, int &w, int &h)
{
	asm("");
}

#endif
#ifdef VORANGEBOX
void __attribute((noinline)) IPrediction::SetupMove(Entity *pl, UserCmd *ucmd, void *movehelper, void *movedata)
{
	asm("");
}

void __attribute((noinline)) IPrediction::FinishMove(Entity *pl, UserCmd *ucmd, void *movedata)
{
	asm("");
}

#endif
#ifdef VL4D
void __attribute((noinline)) IPrediction::SetupMove(Entity *pl, UserCmd *ucmd, void *movehelper, void *movedata)
{
	asm("");
}

void __attribute((noinline)) IPrediction::FinishMove(Entity *pl, UserCmd *ucmd, void *movedata)
{
	asm("");
}

#endif
#ifdef VALIENSWARM
void __attribute((noinline)) IPrediction::SetupMove(Entity *pl, UserCmd *ucmd, void *movehelper, void *movedata)
{
	asm("");
}

void __attribute((noinline)) IPrediction::FinishMove(Entity *pl, UserCmd *ucmd, void *movedata)
{
	asm("");
}

#endif
void __attribute((noinline)) IEngine::GetScreenSize(int &w, int &h)
{
	asm("");
}

bool __attribute((noinline)) IEngine::GetPlayerInfo(int index, player_info &info)
{
	asm("");
}

int __attribute((noinline)) IEngine::GetLocalPlayer()
{
	asm("");
}

bool __attribute((noinline)) IEngine::IsInGame()
{
	asm("");
}

#ifdef VORANGEBOX
matrix4x4 & __attribute((noinline)) IEngine::WorldToScreen()
{
	asm("");
}

#endif
#if defined(VALIENSWARM) || defined(VL4D)
matrix4x4 & __attribute((noinline)) IEngine::WorldToScreen()
{
	asm("");
}

#endif
void __attribute((noinline)) IMovement::ProcessMovement(Entity *pl, void *movedata)
{
	asm("");
}
