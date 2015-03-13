#include "main.h"

ITrace 		*enginetrace;
IClient		*client;
IEngine		*engine;
IEntities	*ents;
IModelInfo	*mdlinfo;
IMovement	*movement;
IPrediction	*prediction;
IEngineVGui *enginevgui;

IPanel		*panel;
ISurface	*surface;

Globals		*globals;

static bool speedfix[0x1000];


declhook(void, PaintTraverse, unsigned int index, bool repaint, bool force)
{
	org_PaintTraverse(t, index, repaint, force);

	static int tpanel = enginevgui->GetPanel(2);
	if (index != tpanel)
		return;

	if (engine->IsInGame())
	{
		hud::DrawESP();
		hud::DrawCrosshair();
	}
}

// static bool speedfix[0x100];

// PØW DÆMØN
// pøw dæmøn

declhook(void, SetViewAngles, Vector &angles)
{
	bpaware();

	register UserCmd *ucmd asm("esi");

#if defined(GMOD)
	if (ucmd && ucmd->tick_count == 0 && ucmd->predicted)
		return;
#endif

	org_SetViewAngles(t, angles);


	if (ucmd && ucmd->command_number == bp->next->arg<int>(0))
	{
		// speedhack code
		// bp->next->next->next->next->ret -= 5;

		// alienswarm engine psilent
		// *((bool *)bp->next - 0x1c)

		// orangebox psilent
		// none yet

		Entity *lp = LocalPlayer();
		Entity *weapon = lp->GetActiveWeapon();

		float curtime = (float)lp->GetTickCount() * globals->interval();

		if (!lp->IsAlive())
			return;


		static int speed = 0;

		if (speedfix[ucmd->command_number % sizeof(speedfix)] = (GetAsyncKeyState(VK_LSHIFT) && speed --> 0))
		{
			bp->next->next->next->next->ret -= 5;
		}
		else
		{
			speed = 4;
		}


		aimbot::RunCommand(ucmd);

		Angle viewang = ucmd->viewangles;
		bool canshoot = weapon && weapon->Clip1() > 0;

#if defined(VORANGEBOX)
		bool &sendpacket = *((bool *)bp->next->next->next - 1);
#endif
#if defined(VL4D)
		bool &sendpacket = *((bool *)bp->next->next->next - 0x21);
#endif
#if defined(VALIENSWARM)
		bool &sendpacket = *((bool *)bp->next - 0x1c);
#endif

		if (1 && !lp->HasFlag(FL_ONGROUND)) // menu/bhop
			ucmd->buttons.del(IN_JUMP);

		if (1 && canshoot) // menu.aimbot
		{
			aimbot::Think(ucmd);
		}

		if (1) // menu.norecoil
		{
			nospread::ApplyRecoil(lp, ucmd->viewangles, -1.0f);
		}


		if (canshoot && ucmd->buttons.test(IN_ATTACK))
		{
			if (1) // menu/nospread
				nospread::ApplySpread(ucmd->command_number, lp, ucmd->viewangles, -1.0f);

			if (1 && weapon->GetNextPrimaryFire() > curtime) // menu/autopistol
				ucmd->buttons.del(IN_ATTACK);

			//if (weapon->GetNextPrimaryFire() <= curtime)
			//	aimbot::Next();
		}

		if (1)
		{
			Angle move;
			VectorAngles(ucmd->move, move);

			float speed = ucmd->move.Length2D();
			float rsin, rcos;
			SinCos(Deg2Rad(ucmd->viewangles.y - viewang.y + move.y), rsin, rcos);

			if (ucmd->viewangles.x < -90.0f || ucmd->viewangles.x > 90.0f)
			{
				ucmd->move.x = rsin * speed;
				ucmd->move.y = rcos * speed;
			}
			else
			{
				ucmd->move.x = rcos * speed;
				ucmd->move.y = rsin * speed;
			}
		}

		// ucmd->move.RotateInPlace();
	}
}

declhook(void, RunCommand, Entity *pl, UserCmd *ucmd, void *movehelper)
{
	if (pl == LocalPlayer())
	{
		using namespace aimbot;
		g_movehelper = movehelper;

		if (ucmd && speedfix[ucmd->command_number % sizeof(speedfix)])
			return RunCommand(ucmd);
	}

	org_RunCommand(t, pl, ucmd, movehelper);
}

extern "C" int __stdcall start(void *, int r, void *)
{
	if (r == 1)
	{
		client = new IClient();
		dtmgr::Start( );

#if defined(VORANGEBOX)
#	if defined(GMOD)
		globals = **(Globals ***)util::FindPattern(client->GetMethod<void *>(0), 0x100, Q"89 0D .? ? ? ? E8");
#	else
#		error OB Globals signature is unknown
#	endif
#else
		globals = **(Globals ***)util::FindPattern(client->GetMethod<void *>(0), 0x100, Q"A3 .? ? ? ? E8");
#endif

		engine  = new IEngine;

		ents 	= new IEntities;
		mdlinfo = new IModelInfo;

		enginetrace = new ITrace;
		
		movement   = new IMovement;
		prediction = new IPrediction;

		panel   = new IPanel;
		surface = new ISurface;
		enginevgui = new IEngineVGui;
		ui::Start();

#if defined(VORANGEBOX)
		engine->HookMethod(20, (void *)hooked_SetViewAngles, (void **)&org_SetViewAngles);
		panel->HookMethod(41, (void *)hooked_PaintTraverse, (void **)&org_PaintTraverse);

		prediction->HookMethod(17, (void *)hooked_RunCommand, (void **)&org_RunCommand);
#elif defined(VL4D)
		engine->HookMethod(20, (void *)hooked_SetViewAngles, (void **)&org_SetViewAngles);
		panel->HookMethod(41, (void *)hooked_PaintTraverse, (void **)&org_PaintTraverse);

		prediction->HookMethod(18, (void *)hooked_RunCommand, (void **)&org_RunCommand);
#endif
	}

	return 1;
}