
#include "misc.h"
#include "notifications.h"

char Config[MAX_PATH];

#ifdef LOGGING
HANDLE LOG_FILE = 0;
#endif

BYTE eCI = 0;

D3D9Info d3d9;
DSound_stuff dsound;
DDraw_stuff ddraw;
D3D9Wnd_settings Settings;
WA_info WA;
Memory_addresses Offsets;
D3D9wnd_environment Env;

BOOL UpdateWACaption()
{
	if (InGame() && Settings.IG.QuickInfo)
	{
		qFileLog("Updating the W:A window caption.");
		char watext[256];

		sprintf_s(watext, "Worms Armageddon (windowed) | %u x %u | %s | %s", WA.BB.Width, WA.BB.Height,
			Settings.Misc.NoTopmost ? "No topmost" : "Topmost", ShowCursorCount() >= 0 ? "Unpinned" : "Pinned");

		return SetWindowText(WA.Wnd.DX, watext);
	}
	return 0;
}

BOOL ReleaseAllPressedKeys()
{
	BYTE keyboard[256];
	BOOL result = GetKeyboardState(keyboard);
	if (result)
	{
		for (int i = 0; i < 256; i++)
		{
			if (keyboard[i] & 0x80)
				SendMessage(WA.Wnd.W2D, WM_KEYUP, i, 0);
		}
	}
	return result;
}

BOOL MinimizeWA()
{
	if (!IsWindowEnabled(WA.Wnd.DX)) EnableWindow(WA.Wnd.DX, 1);

	qFileLog("Minimizing the game via a hook.");

	return ShowWindow(WA.Wnd.DX, SW_MINIMIZE);
}

BOOL ClipCursorInFrontend()
{
	if (!InGame() && !IsNullRect(&WA.Rect.MFC))
	{
		if (WA.BB.Width && WA.BB.Height)
		{
			RECT TestRect = { WA.Rect.MFC.left, WA.Rect.MFC.top, WA.BB.Width, WA.BB.Height };
			if (WA.BB.Width < WA.Rect.MFC.right - WA.Rect.MFC.left && WA.BB.Height < WA.Rect.MFC.bottom - WA.Rect.MFC.top)
				return ClipCursor(&TestRect);
			else
				return ClipCursor(&WA.Rect.MFC);
		}
	}
	
	return false;
}

BOOL StickWnd(HWND hWnd)
{
	RECT WArect;
	GetClientRect(WA.Wnd.DX, &WArect);
	ClientToScreen(WA.Wnd.DX, (POINT*)&WArect);

	return SetWindowPos(hWnd, NULL, WArect.left, WArect.top, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
}

BOOL ChangeTopmostState()
{
	Settings.Misc.NoTopmost = !Settings.Misc.NoTopmost;
	SetWindowPos(WA.Wnd.DX, HWND(-1 - Settings.Misc.NoTopmost), 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);

	UpdateWACaption();

	return Settings.Misc.NoTopmost;
}

BOOL ToggleActiveBackground(BOOL bEnable)
{
	if (bEnable)
	{
		SetParent(WA.Wnd.W2D, WA.Wnd.DX);
		if (!Settings.IG.Stretch && WA.BB.Width < Env.Act.ResX && WA.BB.Height < Env.Act.ResY)
			SetWindowPos(WA.Wnd.W2D, NULL, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
	}
	else
	{
		SetParent(WA.Wnd.W2D, NULL);
		if (!Settings.IG.Stretch && WA.BB.Width < Env.Act.ResX && WA.BB.Height < Env.Act.ResY)
			StickWnd(WA.Wnd.W2D);
	}

	Settings.IG.Background = bEnable;
	UpdateWACaption();
	return TRUE;
}

BOOL FancyUpdate()
{
	BOOL result = 0;
	if (Settings.Misc.FancyStartup)
	{
		result = SetWindowTransparencyLevel(WA.Wnd.DX, eCI * 17);
		if (eCI >= 15) {
			SetWindowLong(WA.Wnd.DX, GWL_EXSTYLE, GetWindowLong(WA.Wnd.DX, GWL_EXSTYLE) &~WS_EX_LAYERED);
			Settings.Misc.FancyStartup = 0;
		}

		fFileLog("FancyStartup update frame %u.", eCI);

		eCI++;
	}
	return result;
}

BOOL D3D9Info::RefreshInterface(IDirect3D9* lpObject)
{
	if (lpObject)
	{
		obj.handle = lpObject;
		obj.CreateDevice = VMTEntry(lpObject, __CreateDevice);
		return 1;
	}
	return 0;
}

BOOL D3D9Info::RefreshDevice(IDirect3DDevice9* lpDevice)
{
	if (lpDevice)
	{
		device.handle = lpDevice;
		device.GetRasterStatus = VMTEntry(lpDevice, __GetRasterStatus);
		device.Release = VMTEntry(lpDevice, __Release);
		device.Reset = VMTEntry(lpDevice, __Reset);
		return 1;
	}
	return 0;
}

HRESULT D3D9Info::CheckFullscreenMode(UINT Adapter, D3DPRESENT_PARAMETERS *pParams)
{
	if (obj.handle)
	{
		if (pParams)
		{
			ULONG dwCount = obj.handle->GetAdapterModeCount(Adapter, pParams->BackBufferFormat);
			D3DDISPLAYMODE DisplayMode;
			for (ULONG i = 0; i < dwCount; i++)
			{
				if (SUCCEEDED(obj.handle->EnumAdapterModes(Adapter, pParams->BackBufferFormat, i, &DisplayMode)))
				if (DisplayMode.Width == pParams->BackBufferWidth && DisplayMode.Height == pParams->BackBufferHeight
					&& DisplayMode.Format == pParams->BackBufferFormat)
				{
					return D3D_OK;
				}
			}
			return D3DERR_NOTAVAILABLE;
		}
		return D3DERR_INVALIDCALL;
	}
	return -1;
}
