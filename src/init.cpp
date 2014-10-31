
#include "misc.h"
#include <d3d9.h>
#include "MinHook\MinHook.h"
#include "tools\tools.h"
#include "init.h"
#include "notifications.h"
#include "hooks_win.h"
#include "hooks_api.h"
#include "hooks_dx.h"

BOOL InitializeD3D9Wnd()
{
	if (!LockCurrentInstance("D3D9Wnd"))
		return 1;

#ifdef LOGGING
	LOG_FILE = CreateFile("d3d9wnd.log", GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
#endif

	qFileLog("DLL_PROCESS_ATTACH: Starting logging.");

	if (CmdOption("-nowindow"))
	{
		qFileLog("Module was disabled via command-line. Suspending...");
		return 1;
	}

	GetPathUnderModule(HINST_THISCOMPONENT, Config, "wkD3D9Wnd.ini");
	Settings.Misc.EnableModule = GetPrivateProfileInt("Misc", "EnableModule", 0, Config);
	if (!Settings.Misc.EnableModule)
	{
		qFileLog("Module was disabled via INI file. Suspending...");
		return 1;
	}

	WA.Version = GetWAVersion();
	fFileLog("Detected the game version: %u.%u.%u.%u",
		PWORD(&WA.Version)[2], PWORD(&WA.Version)[3], PWORD(&WA.Version)[0], PWORD(&WA.Version)[1]);

	if (WA.Version < QV(3,7,2,48))
	{
		if (!EnableDPIAwareness())     qFileLog("The game's process doesn't need to be set DPI-aware.");
		else                           qFileLog("The game's process has been made DPI-aware.");

		if (LoadLibrary("wkWndMode.dll"))
		{
			qFileLog("User has wkWndMode.dll in his game's directory. Suspending...");
			M_WndmodeDetected();
			return 1;
		}

		if ((WA.Steam = SteamCheck()) != 0) qFileLog("User is running the Steam edition of the game.");
		else								qFileLog("User is running the CD edition of the game.");
		
		if (WA.Version < QV(3,6,31,22))
		{
			GetWARegOptionString("LanguageName", walanguage, sizeof(walanguage), "English");
			qFileLog("Too old version of the game. Suspending...");
			M_TooOld();
			return 1;
		}

		Env.Sys.BPP = GetDeviceCaps(GetDC(0), BITSPIXEL);
		if (WA.Version < QV(3,7,2,46) && Env.Sys.BPP < 32)
		{
			fFileLog("Too low colour depth in the system: %u. Notifying the user and suspending...", Env.Sys.BPP);
			M_LowBPP(Env.Sys.BPP);
			return 1;
		}

		if (MH_Initialize() != MH_OK)
		{
			qFileLog("Failed to initialize the MinHook library. Suspending...");
			return 1;
		}

		D3D9CREATE D3DCreate9 = 0;

	#ifdef VISTAUP
		d3d9.dll = GetModuleHandle("d3d9.dll");
		D3DCreate9 = &Direct3DCreate9;
	#else
		if ((d3d9.dll = LoadLibrary("d3d9.dll")) != 0)
		{
			qFileLog("Successfully loaded the d3d9.dll library.");
			if ((D3DCreate9 = (D3D9CREATE)GetProcAddress(d3d9.dll, "Direct3DCreate9")) == 0)
			{
				qFileLog("Failed to find the Direct3DCreate9 entry point in d3d9.dll. Suspending...");
				return 1;
			}
		}
		else
		{
			qFileLog("Failed to load the d3d9.dll library. This system probably doesn't support D3D9. Suspending...");
			return 1;
		}
	#endif

		if (MH_CreateHook(D3DCreate9, Direct3DCreate9Hook, (PVOID*)&Direct3DCreate9Next) != MH_OK)
		{
			qFileLog("Failed to hook Direct3DCreate9! Suspending...");
			return 0;
		}
		else
			qFileLog("Successfully hooked Direct3DCreate9.");

		if (MH_EnableHook(D3DCreate9) != MH_OK)
		{
			qFileLog("Failed to enable the Direct3DCreate9 hook! Suspending...");
			return 0;
		}

		//MH_CreateHook(&DirectDrawCreate, DirectDrawCreateHook, (PVOID*)&DirectDrawCreateNext);
		//MH_CreateHook(&DirectDrawCreateEx, DirectDrawCreateExHook, (PVOID*)&DirectDrawCreateExNext);

		switch (WA.Version)
		{
			case QV(3,7,2,1):
			{
				if (!WA.Steam)
				{
					Offsets.Language = WA.PE.Offset(0x4FF748);
					Offsets.HardwareCursors = WA.PE.Offset(0x4FF700);
					Offsets.ResX = WA.PE.Offset(0x4FF6CC);
					Offsets.ResY = WA.PE.Offset(0x4FF6D0);
					Offsets.RunInBackground = WA.PE.Offset(0x0CBB85);
					Offsets.AfxString = WA.PE.Offset(0x1F1CB0);
					qFileLog("Loaded secondary offsets for the CD edition of 3.7.2.1.");
				}
				else //Steam
				{
					Offsets.Language = WA.PE.Offset(0x4FE740);
					Offsets.HardwareCursors = WA.PE.Offset(0x4FE6F8);
					Offsets.ResX = WA.PE.Offset(0x4FE6C4);
					Offsets.ResY = WA.PE.Offset(0x4FE6C8);
					Offsets.RunInBackground = WA.PE.Offset(0x0CB075); //xB85
					Offsets.AfxString = WA.PE.Offset(0x1F0CB8);

					qFileLog("Loaded secondary offsets offsets for the Steam edition of 3.7.2.1.");
				}
				break;
			}

			case QV(3,7,0,0):
			{
				if (WA.PE.FH->TimeDateStamp != 1355997673)
				{
					fFileLog("User is running 3.7.0.0r1 (%u). Proceeding to the Light mode.", WA.PE.FH->TimeDateStamp);
					Env.Light = true;
					break;
				}

				else
				{
					Offsets.Language = WA.PE.Offset(0x4FD5E0);
					Offsets.HardwareCursors = WA.PE.Offset(0x4FD598);
					Offsets.ResX = WA.PE.Offset(0x4FD564);
					Offsets.ResY = WA.PE.Offset(0x4FD568);
					Offsets.RunInBackground = WA.PE.Offset(0x0CC115);
					Offsets.AfxString = WA.PE.Offset(0x1F0CA8);

					qFileLog("Loaded secondary offsets for the CD edition of 3.7.0.0r2.");
				}
				break;
			}
		
			default:
				qFileLog("Proceeding to the Light initialization mode.");
				Env.Light = true;
				break;
		}

		CheckStaticWindowClass();
		qFileLog("Waiting for the Direct3DCreate9 hook.");
	}
	else
		qFileLog("wkD3D9Wnd is not needed anymore. Bye bye!");
	return 1;
}

BOOL FinalizeD3D9Wnd()
{
	if (WA.Version < QV(3,7,2,38))
		UninstallHooks();
	qFileLog("DLL_PROCESS_DETACH: Bye bye!");
	return 1;
}

void GetMultiMonitorConfig()
{
	Settings.MM.Enable = GetPrivateProfileInt("MultiMonitor", "Enable", 0, Config);
	Settings.MM.Monitors = 1;
	Env.Sys.Monitors = d3d9.obj.handle->GetAdapterCount();
	Env.Act.ResX = Env.Sys.PrimResX;
	Env.Act.ResY = Env.Sys.PrimResY;
	Env.Act.VirtX = 0;
	Env.Act.VirtY = 0;
	if (Settings.MM.Enable)
	{
		char Monitors[MAX_PATH];
		Settings.MM.Auto = GetPrivateProfileInt("MultiMonitor", "AutoSetResolution", 1, Config);
		GetPrivateProfileString("MultiMonitor", "Monitors", "Max", Monitors, MAX_PATH, Config);
		CharLower(Monitors);
		if (!strcmp(Monitors, "max"))
			Settings.MM.Monitors = Env.Sys.Monitors;
		else
			Settings.MM.Monitors = MinCap(MaxCap(atoi(Monitors), Env.Sys.Monitors), 1);
		if (Settings.MM.Monitors >= Env.Sys.Monitors)
		{
			Env.Act.ResX = Env.Sys.VirtResX;
			Env.Act.ResY = Env.Sys.VirtResY;
			Env.Act.VirtX = Env.Sys.VirtX;
			Env.Act.VirtY = Env.Sys.VirtY;
			fFileLog("Multi-monitors are enabled and a maximum of %u is being used. Virtual-screen rectangle: %d,%d;%d,%d",
				Settings.MM.Monitors, Env.Act.VirtX, Env.Act.VirtY, Env.Act.ResX - Env.Act.VirtX, Env.Act.ResY - Env.Act.VirtY);
		}
		else if (Settings.MM.Monitors > 1)
		{
			RECT SuperRect = { 0, 0, 0, 0 };
			for (int i = 0; i < Env.Sys.Monitors; i++)
			{
				MONITORINFO mi;
				mi.cbSize = sizeof(MONITORINFO);
				if (GetMonitorInfo(d3d9.obj.handle->GetAdapterMonitor(i), &mi))
					RectCat(&SuperRect, &mi.rcMonitor);
			}
			Env.Act.ResX = SuperRect.right - SuperRect.left;
			Env.Act.ResY = SuperRect.bottom - SuperRect.top;
			Env.Act.VirtX = SuperRect.left;
			Env.Act.VirtY = SuperRect.top;

			fFileLog("Multi-monitors are enabled: %u total, %u selected by the user. Virtual-screen rectangle: %d,%d;%d,%d",
				Env.Sys.Monitors, Settings.MM.Monitors, SuperRect.left, SuperRect.top, SuperRect.right, SuperRect.bottom);
		}
		else
		{
			Settings.MM.Enable = 0;
			qFileLog("Multi-monitors are enabled by the user, but only 1 monitor was discovered in the system. Using primary desktop resolution.");
		}
	}
	else
		qFileLog("Multi-monitors are disabled by the user. Using primary desktop resolution.");

	D3DCAPS9 caps;
	d3d9.obj.handle->GetDeviceCaps(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, &caps);
	INT32 mw = caps.MaxTextureWidth;
	INT32 mh = caps.MaxTextureHeight;
	if (Env.Act.ResX > mw || Env.Act.ResY > mh)
	{
		fFileLog("Whoopsie! Our resolution of %dx%d exceeds the maximum 2D texture size possible on this system: %ux%u. Fitting to this size.",
			Env.Act.ResX, Env.Act.ResY, mw, mh);
		Env.Act.ResX = MaxCap(Env.Act.ResX, mw);
		Env.Act.ResY = MaxCap(Env.Act.ResX, mh);
	}
	else
		fFileLog("Maximum texture size checked: %ux%u. good to go!", mw, mh);

	if (Settings.MM.Enable && Settings.MM.Auto)
	{
		fFileLog("AutoSetResolution is enabled. Proceeding to setting the %dx%d resolution for in-game.", Env.Act.ResX, Env.Act.ResY);

		if (Env.Light)
		{
			int RegResX = GetWARegOptionDword("DisplayXSize", 0);
			int RegResY = GetWARegOptionDword("DisplayYSize", 0);
			if (RegResX != Env.Act.ResX && RegResY != Env.Act.ResY)
			{
				fFileLog("Registry's resolution is %dx%d: let's change it and inform the user.", RegResX, RegResY);
				WriteWARegOptionDword("DisplayXSize", Env.Act.ResX);
				WriteWARegOptionDword("DisplayYSize", Env.Act.ResY);
				M_SetMultiMonResLight(Env.Act.ResX, Env.Act.ResY);
				qFileLog("Registry multi-monitor resolution has been set, and the user has been prompted to restart the game. W:A PROCESS IS GOING TO EXIT NOW!");
				ExitProcess(0);
			}
			else
				fFileLog("Resolution of %dx%d is already set in registry: no need to update it. Light mode.", Env.Act.ResX, Env.Act.ResY);
		}
		else
		{
			*(DWORD*)Offsets.ResX = Env.Act.ResX;
			*(DWORD*)Offsets.ResY = Env.Act.ResY;

			qFileLog("Set the in-game screen resolution to the total of all of the monitors' resolutions.");
		}
	}
}

void LoadConfig()
{
	GetWALocale();

	Env.Sys.PrimResX = GetSystemMetrics(SM_CXSCREEN);
	Env.Sys.PrimResY = GetSystemMetrics(SM_CYSCREEN);
	Env.Sys.VirtResX = GetSystemMetrics(SM_CXVIRTUALSCREEN);
	Env.Sys.VirtResY = GetSystemMetrics(SM_CYVIRTUALSCREEN);
	Env.Sys.VirtX = GetSystemMetrics(SM_XVIRTUALSCREEN);
	Env.Sys.VirtY = GetSystemMetrics(SM_YVIRTUALSCREEN);

	fFileLog("Detected the primary monitor resolution: %dx%d", Env.Sys.PrimResX, Env.Sys.PrimResY);

	Settings.FR.Fullscreen = GetPrivateProfileInt("FrontendSettings", "Fullscreen", 0, Config);
	Settings.FR.AltFullscreen = GetPrivateProfileInt("FrontendSettings", "FullscreenAlternative", 0, Config);
	Settings.FR.Stretch = GetPrivateProfileInt("FrontendSettings", "Stretch", 0, Config);
	Settings.FR.Centered = GetPrivateProfileInt("FrontendSettings", "Centered", 0, Config);
	Settings.FR.ArbitrarySizing = GetPrivateProfileInt("FrontendSettings", "EnableCustomSize", 0, Config);
	Settings.FR.Xsize = GetPrivateProfileInt("FrontendSettings", "Xsize", 640, Config);
	Settings.FR.Ysize = GetPrivateProfileInt("FrontendSettings", "Ysize", 480, Config);

	Settings.IG.WindowBorder = GetPrivateProfileInt("InGameSettings", "WindowBorder", 0, Config);
	Settings.IG.Stretch = GetPrivateProfileInt("InGameSettings", "Stretch", 0, Config);
	Settings.IG.QuickInfo = GetPrivateProfileInt("InGameSettings", "QuickInfo", 1, Config);
	Settings.IG.TopLeftPosition = GetPrivateProfileInt("InGameSettings", "TopLeftPosition", 0, Config);

	Settings.Misc.SoundInBackground = GetPrivateProfileInt("Misc", "SoundInBackground", 0, Config);
	Settings.Misc.NoTopmost = GetPrivateProfileInt("Misc", "NoTopmost", 1, Config);
	Settings.Misc.FancyStartup = GetPrivateProfileInt("Misc", "FancyStartup", 0, Config);

	if (WA.Version < QV(3,7,2,47))
	{
		Settings.FR.Background = GetPrivateProfileInt("FrontendSettings", "RunInBackground", 1, Config);
		Settings.IG.Background = GetPrivateProfileInt("InGameSettings", "ActiveBackground", 1, Config);
		Settings.IG.AutoUnpin = GetPrivateProfileInt("InGameSettings", "AutoUnpin", 1, Config);
	}
	else
	{
		Settings.FR.Background = 0;
		Settings.IG.Background = 1;
		Settings.IG.AutoUnpin = 0;
	}

	GetMultiMonitorConfig();

	qFileLog("Loaded all settings from the INI file.");

	if (WA.Version < QV(3,7,2,46) && !Settings.FR.Fullscreen && (Settings.FR.Centered || Settings.FR.Stretch || Settings.FR.ArbitrarySizing))
	{
		if (!Env.Light)
		{
			if (*(DWORD*)Offsets.HardwareCursors != 0)
			{
				qFileLog("Hardware cursors are enabled with Centered/Stretched/Custom frontend. Disabling and throwing an info box.");
				PatchMemDword(Offsets.HardwareCursors, 0);
				M_HardwareCursorsDisabled();
			}
		}
		else if (GetWARegOptionDword("UseHardwareCursors", 1))
		{
			qFileLog("Hardware cursors are enabled with Centered/Stretched/Custom frontend. Recommending to disable them with a warning box.");
			M_RecommendHardwareCursorsDisable();
			WriteWARegOptionDword("UseHardwareCursors", 0); //in case registry isn't used, prevent second appearance of the notification
		}
	}

	if (Settings.FR.Background && !Settings.FR.Fullscreen && !Settings.FR.AltFullscreen)
		WriteRunInBackground();
	else
		qFileLog("Running frontend in background is disabled: no need to patch it.");
}

void WriteRunInBackground()
{
	if (Env.Light)
	{
		ULONG RBGSearchResult = FindPatternPrecise((PBYTE)
			"\x8B\x47\x20\x6A\x00\x68\x20\xF0\x00\x00\x68\x12\x01\x00\x00\x50",
			16, WA.PE.Offset(WA.PE.OPT->BaseOfCode), WA.PE.OPT->SizeOfCode);
		if (RBGSearchResult != NULL)
		{
			Offsets.RunInBackground = RBGSearchResult + 11;
			if (PatchMemDword(Offsets.RunInBackground, 0))
				qFileLog("Patched the game in Light mode to prevent it from self-minimizing.");
			else
				qFileLog("Something went WRONG when patching the game in Light mode to prevent it from self-minimizing!");
		}
		else
			qFileLog("Failed to find where to patch RunInBackground in Light mode!");
	}
	else
	if (PatchMemDword(Offsets.RunInBackground, 0))
		qFileLog("Patched the game in predefined mode to prevent it from self-minimizing.");
	else
		qFileLog("Something went WRONG when patching the game in predefined mode to prevent it from self-minimizing!");
}

void CheckStaticWindowClass()
{
	Settings.Misc.StaticWindowClass = GetPrivateProfileInt("Misc", "StaticWindowClass", 1, Config);
	if (Settings.Misc.StaticWindowClass)
	{
		qFileLog("Static window class is enabled: preparing to patch.");
		if (Env.Light)
		{
			ULONG AFXSearchResult = FindPatternPrecise((PBYTE)"Afx:%p:%x:%p:%p:%p",
				18, WA.PE.Offset(WA.PE.OPT->BaseOfData), WA.PE.OPT->SizeOfInitializedData);
			if (AFXSearchResult != NULL)
			{
				Offsets.AfxString = AFXSearchResult;
				if (PatchMemStringA(Offsets.AfxString, "ArmageddonWindowed"))
					qFileLog("Patched the game's window class in Light mode.");
				else
					qFileLog("Something went WRONG when patching the game's window class in Light mode!");
			}
			else
				qFileLog("Failed to find the window class string in Light mode!");
		}
		else
		{
			if (PatchMemStringA(Offsets.AfxString, "ArmageddonWindowed"))
				qFileLog("Patched the game's window class in predefined mode.");
			else
				qFileLog("Something went WRONG when patching the game's window class in predefined mode!");
		}
	}
	else
		qFileLog("Static window class is disabled: no need to enable the hook early.");
}

void GetWALocale()
{
	if (Env.Light)
		GetWARegOptionString("LanguageName", walanguage, sizeof(walanguage), "English");
	else
		strcpy_s(walanguage, *(PCHAR*)Offsets.Language);
}

BOOL Hooked = 0;
BOOL __stdcall InstallHooks()
{
	qFileLog("InstallHooks: Proceeding to installing the hooks.");

	if (!Hooked)
	{
		//MH_RemoveHook(&DirectDrawCreate);
		//MH_RemoveHook(&DirectDrawCreateEx);

		hkb = SetWindowsHookEx(WH_KEYBOARD, KeyboardProc, 0, GetCurrentThreadId());
		if (hkb)                           qFileLog("InstallHooks: Successfully installed a WH_KEYBOARD Windows hook.");
		else                               qFileLog("InstallHooks: FAILED to install a WH_KEYBOARD Windows hook!");

		hkb2 = SetWindowsHookEx(WH_CALLWNDPROC, CallWndProc, 0, GetCurrentThreadId());
		if (hkb2)                      qFileLog("InstallHooks: Successfully installed a WH_CALLWNDPROC Windows hook.");
		else                           qFileLog("InstallHooks: FAILED to install a WH_CALLWNDPROC Windows hook!");

		if (Settings.IG.AutoUnpin)
		{
			hkb3 = SetWindowsHookEx(WH_MOUSE, MouseProc, 0, GetCurrentThreadId());
			if (hkb3)                      qFileLog("InstallHooks: Successfully installed a WH_MOUSE Windows hook.");
			else                           qFileLog("InstallHooks: FAILED to install a WH_MOUSE Windows hook!");
		}

		if (WA.Version < QV(3,7,1,1))
		{
			if (MH_CreateHook(&SetForegroundWindow, SetForegroundWindowNew, (PVOID*)&SetForegroundWindowNext) != MH_OK)
				qFileLog("InstallHooks: FAILED to hook SetForegroundWindow!");
			else qFileLog("InstallHooks: Successfully hooked SetForegroundWindow.");
		}

		if (MH_CreateHook(&CreateWindowExA, CreateWindowExANew, (PVOID*)&CreateWindowExANext) != MH_OK)
			qFileLog("InstallHooks: FAILED to hook CreateWindowExA!");
		else
			qFileLog("InstallHooks: Successfully hooked CreateWindowExA.");

		if (MH_CreateHook(&MoveWindow, MoveWindowNew, (PVOID*)&MoveWindowNext) != MH_OK)
			qFileLog("InstallHooks: FAILED to hook MoveWindow!");
		else
			qFileLog("InstallHooks: Successfully hooked MoveWindow.");
		if (MH_CreateHook(&SetCursorPos, SetCursorPosNew, (PVOID*)&SetCursorPosNext) != MH_OK)
			qFileLog("InstallHooks: FAILED to hook SetCursorPos!");
		else
			qFileLog("InstallHooks: Successfully hooked SetCursorPos.");

		if (d3d9.obj.handle && !D3D9CreateDeviceNext)
		if (MH_CreateHook(d3d9.obj.CreateDevice, D3D9CreateDeviceHook, (PVOID*)&D3D9CreateDeviceNext) != MH_OK)
			qFileLog("InstallHooks: FAILED to hook IDirect3D9::CreateDevice!");

		if (Settings.Misc.SoundInBackground && !dsound.dll)
		{
			qFileLog("SoundInBackground is enabled: hooking the necessary things.");
			DSOUNDCREATE DSoundCreate = 0;
			BOOL n = 0;
			do{
#ifdef VISTAUP
				dsound.dll = GetModuleHandle("dsound.dll");
				DSoundCreate = &DirectSoundCreate;
#else
				if ((dsound.dll = LoadLibrary("dsound.dll")) != 0)
				{
					qFileLog("Successfully loaded the dsound.dll library.");
					if ((DSoundCreate = (DSOUNDCREATE)GetProcAddress(dsound.dll, "DirectSoundCreate")) == 0)
					{
						qFileLog("Failed to find the DirectSoundCreate entry point in dsound.dll. Sound hooks will be disabled.");
						break;
					}
				}
				else
				{
					qFileLog("Failed to load the dsound.dll library! Sound hooks will be disabled.");
					break;
				}
#endif
				if (MH_CreateHook(DSoundCreate, DSoundCreateHook, (PVOID*)&DSoundCreateNext) != MH_OK)
					qFileLog("InstallHooks: FAILED to hook DirectSoundCreate!");
				else
					qFileLog("InstallHooks: Successfully hooked DirectSoundCreate.");
			} while (n);
		}

		if (MH_EnableHook(MH_ALL_HOOKS) != MH_OK)
			qFileLog("InstallHooks: FAILED to enable all Hooks!");

		return Hooked = TRUE;
	}
	else
		qFileLog("InstallHooks: Hooks were already installed.");
	return 0;
}

BOOL __stdcall UninstallHooks()
{
	qFileLog("UninstallHooks: Proceeding to uninstalling the hooks.");

	if (MH_DisableHook(MH_ALL_HOOKS) == MH_OK)
	{
		UnhookWindowsHookEx(hkb);
		UnhookWindowsHookEx(hkb2);
		if (Settings.IG.AutoUnpin) UnhookWindowsHookEx(hkb3);
		SetForegroundWindowNext = 0;
		D3D9GetRasterStatusNext = 0;
		D3D9ReleaseNext = 0;
		D3D9PresentNext = 0;
		D3D9CreateDeviceNext = 0;
#ifndef VISTAUP
		if (d3d9.dll) { FreeLibrary(d3d9.dll); d3d9.dll = 0; }
		if (dsound.dll) { FreeLibrary(dsound.dll); dsound.dll = 0; }
#endif
		Hooked = FALSE;
		qFileLog("UninstallHooks: Successfully disabled all hooks.");
		return 1;
	}
	else
		qFileLog("UninstallHooks: FAILED to disable all hooks!");
	return 0;
}

