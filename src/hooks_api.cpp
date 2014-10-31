
#include "hooks_api.h"
#include "tools\tools.h"
#include "misc.h"
#include "notifications.h"

HWND(WINAPI *CreateWindowExANext)(DWORD, LPCTSTR, LPCTSTR, DWORD, int, int, int, int, HWND, HMENU, HINSTANCE, LPVOID);
BOOL(WINAPI *SetForegroundWindowNext)(HWND);
BOOL(WINAPI *SetCursorPosNext)(int, int);
BOOL(WINAPI *MoveWindowNext)(HWND, int, int, int, int, BOOL);

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
