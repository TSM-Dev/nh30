#include "hud.h"

extern IEngine *engine;
extern IEntities *ents;

extern ISurface *surface;

extern Globals *globals;


extern int m_angRotation;
extern int m_Collision;
extern int m_vecMins, m_vecMaxs;
extern int m_mobRush;

extern int m_nSolidType, m_usSolidFlags;

bool hud::MakeBox(Entity *ent, int &x0, int &y0, int &x1, int &y1)
{
	Vector edges[4] =
	{
		Vector(1.0f , 1.0f, 1.0f),
		Vector(-1.0f, 1.0f, 1.0f),
		Vector(1.0f ,-1.0f, 1.0f),
		Vector(-1.0f,-1.0f, 1.0f),
	};

	Vector vec = ent->GetPos();
	Angle  ang = ReadPtr<Angle>(ent, m_angRotation);

	Vector mins = ReadPtr<Vector>(ent, m_Collision + m_vecMins);
	Vector maxs = ReadPtr<Vector>(ent, m_Collision + m_vecMaxs);

	x0 = maxof(int);
	y0 = maxof(int);
	x1 = minof(int);
	y1 = minof(int);

	for (int j = 0; j < 4; j++)
	{
		Vector mins2d, maxs2d;

		if ((vec + (mins * edges[j].Rotate(ang))).ToScreen(mins2d) == false)
			return false;

		if ((vec + (maxs * edges[j].Rotate(ang))).ToScreen(maxs2d) == false)
			return false;

		x0 = min(x0, min(mins2d.x, maxs2d.x));
		y0 = min(y0, min(mins2d.y, maxs2d.y));
		x1 = max(x1, max(mins2d.x, maxs2d.x));
		y1 = max(y1, max(mins2d.y, maxs2d.y));
	}

	return true;
}

void hud::DrawESP()
{
	surface->SetFont(ui::font_hud);
	surface->SetTextColor(0xff, 0xff, 0xff, 0xff);

	Entity *lp = LocalPlayer();
	int  maxent = globals->maxclients();

	for (int i = 1; i <= ents->GetMaxEntities(); ++i)
	{
		Entity *ent = ents->GetEntity(i);

		if (!ent || ent == lp)
			continue;

		if (ent->IsDormant())
			continue;

		if (1 && i <= maxent)
		{
			if (!ent->IsAlive())
				continue;


			int x0, y0, x1, y1;

			if (!MakeBox(ent, x0, y0, x1, y1))
				continue;

			surface->SetColor(0x00, 0x00, 0x00, 0x7f);
			surface->DrawOutlinedRect(x0 - 1, y0 - 1, x1 + 1, y1 + 1);
			surface->DrawOutlinedRect(x0 + 1, y0 + 1, x1 - 1, y1 - 1);

			if (1) // menu.ESP.health
				surface->DrawFilledRect(x0 - 4, y0 - 1, x0 - 1, y1 + 1);

#if defined(L4D) || defined(L4D2)
			if (ent->GetTeam() == 2 || ent->GetTeam() == 4)
				surface->SetColor(0x46, 0x78, 0xff, 0xff);

			if (ent->GetTeam() == 3)
				surface->SetColor(0x96, 0x00, 0xe1, 0xff);
#else
			if (ent->GetTeam() == TEAM_RED)
				surface->SetColor(0xff, 0x64, 0x64, 0xff);

			if (ent->GetTeam() == TEAM_BLU)
				surface->SetColor(0x46, 0x78, 0xff, 0xff);
#endif

			surface->DrawOutlinedRect(x0, y0, x1, y1);

			if (1) // menu.ESP.health
			{
				if (ent->GetHealth() <= ent->GetMaxHealth())
				{
					if (ent->GetHealth() <= (ent->GetMaxHealth() / 4))
						surface->SetColor(0xff, 0x00, 0x00, 0xff);
					else
						surface->SetColor(0x00, 0xff, 0x00, 0xff);
				}

				surface->DrawFilledRect(x0 - 3, y1 - (int)((y1 - y0) * min((float)ent->GetHealth() / (float)ent->GetMaxHealth(), 1.0f) + 0.5f), x0 - 1, y1);
			}

			if (1) // menu.ESP.name
			{
				static player_info info;
				engine->GetPlayerInfo(i, info);

				int length;
				const wchar_t *text = tounicode(info.name, length);

				int tw, th;
				surface->GetTextSize(ui::font_hud, text, tw, th);

				surface->SetTextPos((x0 + x1 - tw) / 2, y0 - th);
				surface->DrawText(text, length, 0);
			}

			if (1) // menu.ESP.weapon
			{
				Entity *weapon = ent->GetActiveWeapon();

				if (weapon)
				{
					const char *classname = weapon->GetClass();
#if !defined(GMOD)
					if (strlen(classname) > 6)
					{
						classname += 6;
					}
#endif

					int length;
					const wchar_t *text = util::MakeReadable(classname, length);

					int tw, th;
					surface->GetTextSize(ui::font_hud, text, tw, th);

					surface->SetTextPos((x0 + x1 - tw) / 2, y1 + 1);
					surface->DrawText(text, length, 0);
				}
			}

			continue;
		}

#if defined(L4D) || defined(L4D2)
		if (1 && !strcmp(ent->GetNetworkClass()->name, "Infected"))
		{
			if (!ent->NPC_IsAlive())
				continue;

			int x0, y0, x1, y1;

			if (!MakeBox(ent, x0, y0, x1, y1))
				continue;

			surface->SetColor(0x00, 0x00, 0x00, 0x7f);
			surface->DrawOutlinedRect(x0 - 1, y0 - 1, x1 + 1, y1 + 1);
			surface->DrawOutlinedRect(x0 + 1, y0 + 1, x1 - 1, y1 - 1);

			surface->SetColor(0xc8, 0xff, 0x00, 0xff);
			surface->DrawOutlinedRect(x0, y0, x1, y1);


			int tw, th;

			if (ReadPtr<bool>(ent, m_mobRush))
			{
				surface->GetTextSize(ui::font_hud, L"Mob rush", tw, th);

				surface->SetTextPos((x0 + x1 - tw) / 2, y1 + 1);
				surface->DrawText(L"Mob rush", 8, 0);
			}
			else
			{
				surface->GetTextSize(ui::font_hud, L"Common", tw, th);

				surface->SetTextPos((x0 + x1 - tw) / 2, y1 + 1);
				surface->DrawText(L"Common", 6, 0);
			}

			continue;
		}
#endif
	}
}

void hud::DrawCrosshair()
{
	int x, y;
	engine->GetScreenSize(x, y);

	x /= 2;
	y /= 2;

	surface->SetColor(0xff, 0xff, 0xff, 0xff);
	surface->DrawFilledRect(x - 2, y - 1, x, y + 1);
	surface->DrawFilledRect(x, y - 1, x + 2, y + 1);
	surface->DrawFilledRect(x - 1, y - 2, x + 1, y);
	surface->DrawFilledRect(x - 1, y, x + 1, y + 2);
}