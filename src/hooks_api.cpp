
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
	
	if (!Settings.FR.Fullscreen || Settings.FR.AltFullscreen)
	{
		SetWindowLong(Wnd, GWL_EXSTYLE, GetWindowLong(Wnd, (GWL_EXSTYLE)) | WS_EX_LAYERED | WS_EX_COMPOSITED);
		SetLayeredWindowAttributes(Wnd, 0, 1, LWA_ALPHA);
		//GDI layer workaround: prevent white mess on focus loss and maximization in frontend screens.

		qFileLog("CreateDialogIndirectParam: Applied the transparency layer to the new MFC dialog.");
	}

	if (hWndParent == WA.Wnd.DX)
	{
		qFileLog("CreateDialogIndirectParam: A generic frontend MFC dialog screen has been entered.");

		if (!IsWindowEnabled(WA.Wnd.DX))
			EnableWindow(WA.Wnd.DX, true); //idk

		if (!Settings.FR.Fullscreen && !Settings.FR.AltFullscreen && !Settings.MM.Enable
			&& (Settings.FR.Stretch || Settings.FR.ArbitrarySizing || Settings.FR.Centered))
		{
			RECT WRect = { 0, 0, WA.BB.Width, WA.BB.Height };
			ClipCursor(&WRect);
		}

		Env.EasterEgg.FrontendHidden = false;
		WA.Wnd.MFC = Wnd;
		GetWindowRect(Wnd, &WA.Rect.MFC);
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
			qFileLog("MoveWindow: Preparing to move the frontend window.");

			if (IsWindow(WA.Wnd.MFC))
			{
				SetWindowPos(WA.Wnd.MFC, NULL, WA.Rect.MFC.left, WA.Rect.MFC.top, WA.Rect.MFC.right-WA.Rect.MFC.left, WA.Rect.MFC.bottom-WA.Rect.MFC.top, SWP_NOMOVE | SWP_NOZORDER);
				ShowWindow(WA.Wnd.MFC, SW_SHOW);
				qFileLog("MoveWindow: Restored the last MFC overlay to its initial size and display.");
			}

			if (Settings.FR.Fullscreen || Settings.FR.AltFullscreen)
			{
				SetWindowText(WA.Wnd.DX, "Worms Armageddon");

				qFileLog("MoveWindow: Calling next hook. We are in fullscreen frontend mode: no further adjustments required.");
				return MoveWindowNext(hWnd, X, Y, nWidth, nHeight, bRepaint);
			}
			else
				SetWindowText(WA.Wnd.DX, "Worms Armageddon (windowed)");

			RECT WRect = { 0, 0, WA.BB.Width, WA.BB.Height };

			if (Settings.FR.Stretch)
			{
				if (!Settings.MM.Enable)
					ClipCursor(&WRect);

				X = 0;
				Y = 0;
				nWidth = Env.Sys.PrimResX;
				nHeight = Env.Sys.PrimResY;

				qFileLog("MoveWindow: Calling next hook. Stretch mode.");
			}

			else if (Settings.FR.Centered)
			{
				if (Settings.FR.ArbitrarySizing)
				{
					X = MinCap((Env.Sys.PrimResX / 2) - (Settings.FR.Xsize / 2), 0);
					Y = MinCap((Env.Sys.PrimResY / 2) - (Settings.FR.Ysize / 2), 0);
					nWidth = MaxCap(Settings.FR.Xsize, Env.Sys.PrimResX);
					nHeight = MaxCap(Settings.FR.Ysize, Env.Sys.PrimResY);

					qFileLog("MoveWindow: Calculated the position of CenteredFrontend with CustomSize enabled.");
				}

				else
				{
					ClipCursor(&WRect);
					X = MinCap((Env.Sys.PrimResX / 2) - (WA.BB.Width / 2), 0);
					Y = MinCap((Env.Sys.PrimResY / 2) - (WA.BB.Height / 2), 0);

					qFileLog("MoveWindow: Calculated the position of CenteredFrontend.");
				}
			}

			else if (Settings.FR.ArbitrarySizing)
			{
				ClipCursor(&WRect);
				nWidth = MaxCap(Settings.FR.Xsize, Env.Sys.PrimResX);
				nHeight = MaxCap(Settings.FR.Ysize, Env.Sys.PrimResY);

				qFileLog("MoveWindow: Calling next hook. Calculated the position with CustomSize enabled.");
			}
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
