
#include "hooks_api.h"
#include "tools\tools.h"
#include "misc.h"
#include "notifications.h"

HWND(WINAPI *CreateDialogIndirectParamANext)(HINSTANCE, LPCDLGTEMPLATEA, HWND, DLGPROC, LPARAM);
HWND(WINAPI *CreateWindowExANext)(DWORD, LPCTSTR, LPCTSTR, DWORD, int, int, int, int, HWND, HMENU, HINSTANCE, LPVOID);
BOOL(WINAPI *SetForegroundWindowNext)(HWND);
BOOL(WINAPI *SetCursorPosNext)(int, int);
BOOL(WINAPI *MoveWindowNext)(HWND, int, int, int, int, BOOL);

HWND WINAPI CreateDialogIndirectParamANew(HINSTANCE hInstance,LPCDLGTEMPLATEA lpTemplate,HWND hWndParent,DLGPROC lpDialogFunc,LPARAM dwInitParam)
{
	HWND Wnd = CreateDialogIndirectParamANext(hInstance, lpTemplate, hWndParent, lpDialogFunc, dwInitParam);

	if (WA.Version < QV(3,7,2,46) && (!Settings.FR.Fullscreen || Settings.FR.AltFullscreen))
	{
		EnableWindow(WA.Wnd.DX, true);
		SetWindowLong(Wnd, GWL_EXSTYLE, GetWindowLong(Wnd, (GWL_EXSTYLE)) | WS_EX_LAYERED | WS_EX_COMPOSITED);
		SetLayeredWindowAttributes(Wnd, 0, 1, LWA_ALPHA);
		//GDI layer workaround: prevent white mess on focus loss and maximization in frontend screens.

		qFileLog("CreateDialogIndirectParam: Applied the transparency layer to the new MFC dialog and enabled the DX window.");
	}

	if (hWndParent == WA.Wnd.DX)
	{
		qFileLog("CreateDialogIndirectParam: A generic frontend MFC dialog screen has been entered.");

		Env.EasterEgg.FrontendHidden = false;
		WA.Wnd.MFC = Wnd;
		GetWindowRect(Wnd, &WA.Rect.MFC);

		if (WA.Version < QV(3,7,2,46) && (!Settings.FR.Fullscreen && !Settings.FR.AltFullscreen && !Settings.MM.Enable
			&& (Settings.FR.Stretch || Settings.FR.ArbitrarySizing || Settings.FR.Centered)))
		{
			ClipCursorInFrontend();
		}
	}
	ShowWindow(Wnd, SW_SHOW);
	return Wnd;
}

HWND WINAPI CreateWindowExANew(DWORD dwExStyle, LPCTSTR lpClassName, LPCTSTR lpWindowName, DWORD dwStyle, int x, int y, 
	int nWidth, int nHeight, HWND hWndParent, HMENU hMenu, HINSTANCE hInstance, LPVOID lpParam)
{
	if (lpClassName > (PVOID)65535 && lpWindowName)
	{
		if (!strcmp(lpClassName, "Worms2D") && !strcmp(lpWindowName, "Worms2D"))
		{
			qFileLog("CreateWindowEx: The Worms2D widow was summoned.");
			return WA.Wnd.W2D = CreateWindowExANext(dwExStyle, lpClassName, lpWindowName, dwStyle, x, y, nWidth, nHeight, hWndParent, hMenu, hInstance, lpParam);
		}
	}

	return CreateWindowExANext(dwExStyle, lpClassName, lpWindowName, dwStyle, x, y, nWidth, nHeight, hWndParent, hMenu, hInstance, lpParam);
}

BOOL WINAPI SetForegroundWindowNew(HWND hWnd) //for 3.7.0.0
{
	if (hWnd == GetDesktopWindow())
		if (InGame())
			return MinimizeWA();
	
	return SetForegroundWindowNext(hWnd);
}

BOOL WINAPI SetCursorPosNew(int X, int Y)
{
	if (InGame())
	{
		if (!IsIconic(WA.Wnd.DX)) GetWindowRect(WA.Wnd.W2D, &WA.Rect.W2D);
		X += WA.Rect.W2D.left;
		Y += WA.Rect.W2D.top;
	}

	return SetCursorPosNext(X,Y);
}

BOOL WINAPI MoveWindowNew(HWND hWnd, int X, int Y, int nWidth, int nHeight, BOOL bRepaint)
{
	if (hWnd == WA.Wnd.DX)
	{
		if (!InGame())
		{
			return TRUE;
		}
	}

	else if (hWnd == WA.Wnd.W2D)
	{
		if (Settings.IG.Stretch || nWidth >= Env.Act.ResX || nHeight >= Env.Act.ResY)
		{
			X = Env.Act.ResX/2 - nWidth/2;
			Y = Env.Act.ResY/2 - nHeight/2;
		}

		else if (!Settings.IG.Background && nWidth < Env.Act.ResX && nHeight < Env.Act.ResY)
		{
			RECT WArect;
			GetClientRect(WA.Wnd.DX,&WArect);
			ClientToScreen(WA.Wnd.DX,(POINT*)&WArect);
			X = WArect.left;
			Y = WArect.top;
		}

		fFileLog("MoveWindow: Performed the necessary Worms2D window position adjustments if required. X: %d, Y: %d", X, Y);
	}

	return MoveWindowNext(hWnd,X,Y,nWidth,nHeight,bRepaint);
}
