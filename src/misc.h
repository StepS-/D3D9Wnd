#pragma once

#include <Windows.h>
#include <stdio.h>
#include <d3d9.h>
#include <dsound.h>
#include "tools\tools.h"

#ifdef LOGGING
extern HANDLE LOG_FILE;
#define fFileLog(Format, ...) LogToFileA(LOG_FILE, Format, __VA_ARGS__)
#define qFileLog(string) LogToFileA(LOG_FILE, "%s", string)
#else
#pragma warning( disable : 4390 )
#define fFileLog(Format, ...)
#define qFileLog(string)
#endif

#define InGame() IsWindow(WA.Wnd.W2D)

//d3d9
#define __CreateDevice 16

//d3ddevice9
#define __Release 2
#define __TestCooperativeLevel 3
#define __Reset 16
#define __Present 17
#define __GetRasterStatus 19

//dsound
#define __CreateSoundBuffer 3
#define __SetCooperativeLevel 6

enum MMSetupType
{
	MULTIMON_TYPE_NONE,
	MULTIMON_TYPE_HORIZONTAL,
	MULTIMON_TYPE_VERTICAL,
	MULTIMON_TYPE_RECTANGLE,
};

struct D3D9Info{
	BOOL RefreshInterface(IDirect3D9* lpObject);
	BOOL RefreshDevice(IDirect3DDevice9* lpDevice);
	HRESULT CheckFullscreenMode(UINT Adapter, D3DPRESENT_PARAMETERS *pParams);

	HMODULE dll;
	struct{
		IDirect3D9* handle;
		PVOID
			CreateDevice;
	} obj;
	struct{
		IDirect3DDevice9* handle;
		PVOID
			Reset,
			GetRasterStatus,
			Release;
	} device;
};

struct DSound_stuff{
	HMODULE dll;
	IDirectSound* obj;
};

struct DDraw_stuff {
	HMODULE dll;
};

struct D3D9Wnd_settings{
	struct{
		BOOL
			EnableModule,
			SoundInBackground,
			StaticWindowClass,
			NoTopmost,
			FancyStartup;
	} Misc;
	struct{
		BOOL
			Fullscreen,
			AltFullscreen,
			Stretch,
			Centered,
			Background,
			ArbitrarySizing,
			ArbitrarySizingNecessary;
		INT32
			Xsize,
			Ysize;
	} FR;
	struct{
		BOOL
			Fullscreen,
			WindowBorder,
			Stretch,
			QuickInfo,
			Background,
			TopLeftPosition,
			AutoUnpin;
	} IG;
	struct{
		INT32
			Monitors;
		BOOL
			Enable,
			Auto;
	} MM;
};

struct WA_info{
	QWORD Version;
	bool Steam;
	struct Windows{
		HWND
			DX,
			W2D,
			MFC;
	} Wnd;
	struct BackBuffer{
		INT32
			Width,
			Height;
		BackBuffer() :
			Width(640),
			Height(480) {}
	} BB;
	struct KeptRects{
		RECT
			DX,
			W2D,
			MFC;
	} Rect;
	PEInfo PE;
};

struct Memory_addresses{
	DWORD
		RunInBackground,
		Language,
		HardwareCursors,
		AfxString,
		ResX,
		ResY;
};

struct D3D9wnd_environment{
	BOOL Light, FrontInit, DDraw, AVSAdjust;
	struct SysValues{
		INT32
			VirtResX,
			VirtResY,
			VirtX,
			VirtY,
			PrimResX,
			PrimResY,
			BPP,
			Monitors,
			DpiAwareness;
	} Sys;
	struct CustomValues{
		INT32
			ResX,
			ResY,
			VirtX,
			VirtY;
	} Act;
	struct WaitFlags{
		BOOL ResetFullscreenInGame;
		BOOL NeedsCentering;
	} Queue;
	struct EasterEggData{
		BOOL
			FrontendHidden,
			InControl;
		INT32
			Multiplier;
		EasterEggData() :
			Multiplier(1) {}
	} EasterEgg;
};

extern char Config[MAX_PATH];

extern D3D9Wnd_settings Settings;
extern WA_info WA;
extern Memory_addresses Offsets;
extern DSound_stuff dsound;
extern DDraw_stuff ddraw;
extern D3D9wnd_environment Env;

extern D3D9Info d3d9;

BOOL UpdateWACaption();
BOOL ReleaseAllPressedKeys();
BOOL MinimizeWA();
BOOL ClipCursorInFrontend();
BOOL ChangeTopmostState();
BOOL FancyUpdate();
BOOL StickWnd(HWND hWnd);
BOOL ToggleActiveBackground(BOOL bEnable);
