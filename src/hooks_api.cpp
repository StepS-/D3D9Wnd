
#include "hooks_api.h"
#include "tools\tools.h"
#include "misc.h"
#include "notifications.h"

HWND(WINAPI *CreateDialogIndirectParamANext)(HINSTANCE, LPCDLGTEMPLATEA, HWND, DLGPROC, LPARAM);
HWND(WINAPI *CreateWindowExANext)(DWORD, LPCTSTR, LPCTSTR, DWORD, int, int, int, int, HWND, HMENU, HINSTANCE, LPVOID);
BOOL(WINAPI *SetForegroundWindowNext)(HWND);
BOOL(WINAPI *IsIconicNext)(HWND);
BOOL(WINAPI *SetCursorPosNext)(int, int);
BOOL(WINAPI *MoveWindowNext)(HWND, int, int, int, int, BOOL);
BOOL(WINAPI *SetWindowPosNext)(HWND, HWND, int, int, int, int, UINT);

HWND WINAPI CreateDialogIndirectParamANew(HINSTANCE hInstance, LPCDLGTEMPLATEA lpTemplate, HWND hWndParent, DLGPROC lpDialogFunc, LPARAM dwInitParam)
{
	HWND Wnd = CreateDialogIndirectParamANext(hInstance, lpTemplate, hWndParent, lpDialogFunc, dwInitParam);
	if (hWndParent == WA.Wnd.DX)
	{
		qFileLog("WindowProc: A generic frontend MFC dialog screen has been entered.");

		Env.EasterEgg.FrontendHidden = false;
		WA.Wnd.MFC = Wnd;
		GetWindowRect(Wnd, &WA.Rect.MFC);

		if (WA.Version < QV(3,7,2,46) && (!Settings.FR.Fullscreen && !Settings.FR.AltFullscreen && !Settings.MM.Enable
			&& (Settings.FR.Stretch || Settings.FR.ArbitrarySizing || Settings.FR.Centered)))
		{
			ClipCursorInFrontend();
		}
	}
	return Wnd;
}

BOOL WINAPI SetWindowPosNew(HWND hWnd, HWND hWndInsertAfter, int X, int Y, int cx, int cy, UINT uFlags)
{
	BOOL result = SetWindowPosNext(hWnd, hWndInsertAfter, X, Y, cx, cy, uFlags);
	if (!result)
		fFileLog("SetWindowPos has FAILED! Error: 0x%X", GetLastError());
	return result;
}

HWND WINAPI CreateWindowExANew(DWORD dwExStyle, LPCTSTR lpClassName, LPCTSTR lpWindowName, DWORD dwStyle, int x, int y, 
	int nWidth, int nHeight, HWND hWndParent, HMENU hMenu, HINSTANCE hInstance, LPVOID lpParam)
{
	if (lpClassName > (PVOID)65535 && lpWindowName)
	{
		if (!strcmp(lpClassName, "Worms2D") && !strcmp(lpWindowName, "Worms2D"))
		{
			dwExStyle |= WS_EX_TRANSPARENT;
			qFileLog("CreateWindowEx: The Worms2D window was summoned and pwned.");
			return WA.Wnd.W2D = CreateWindowExANext(dwExStyle, lpClassName, lpWindowName, dwStyle, x, y, nWidth, nHeight, hWndParent, hMenu, hInstance, lpParam);
		}
	}

	return CreateWindowExANext(dwExStyle, lpClassName, lpWindowName, dwStyle, x, y, nWidth, nHeight, hWndParent, hMenu, hInstance, lpParam);
}

BOOL WINAPI SetForegroundWindowNew(HWND hWnd) //for 3.7.0.0
{
	if (hWnd == GetDesktopWindow())
		if (InGame() && !Settings.IG.Fullscreen)
			return MinimizeWA();
	
	return SetForegroundWindowNext(hWnd);
}

BOOL WINAPI IsIconicNew(HWND hWnd)
{
	if (InGame() && !Settings.IG.Fullscreen && hWnd == GetParent(WA.Wnd.W2D))
		return TRUE; //fix high CPU usage on minimization in some older versions
	return IsIconicNext(hWnd);
}

BOOL WINAPI SetCursorPosNew(int X, int Y)
{
	if (InGame() && !Settings.IG.Fullscreen)
	{
		GetWindowRect(WA.Wnd.W2D, &WA.Rect.W2D);
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

	else if (hWnd == WA.Wnd.W2D && !Settings.IG.Fullscreen)
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
