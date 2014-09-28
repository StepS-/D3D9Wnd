
#include "hooks_dx.h"
#include "MinHook\MinHook.h"
#include "tools\tools.h"
#include "misc.h"
#include "notifications.h"
#include "init.h"

#ifdef LOGGING
ULONG PIPI, PIGI;
#endif

//HRESULT(WINAPI *DirectDrawCreateNext)(GUID*, LPDIRECTDRAW*, IUnknown*);
//HRESULT(WINAPI *DirectDrawCreateExNext)(GUID*, LPDIRECTDRAW*, REFIID, IUnknown*);
//FARPROC (WINAPI *GetProcAddressNext)(HMODULE, LPCSTR);

D3D9CREATE Direct3DCreate9Next;
D3D9_CREATEDEVICE D3D9CreateDeviceNext;
D3D9DEVICE_PRESENT D3D9PresentNext;
D3D9DEVICE_TESTCOOPERATIVELEVEL D3D9TestCooperativeLevelNext;
D3D9DEVICE_RESET D3D9ResetNext;
D3D9DEVICE_GETRASTERSTATUS D3D9GetRasterStatusNext;
IUNKNOWN_RELEASE D3D9ReleaseNext;
DSOUNDCREATE DSoundCreateNext;
DSOUND_CREATESOUNDBUFFER DSoundCreateSoundBufferNext;

ULONG WINAPI D3D9ReleaseHook(IUnknown* pthis)
{
	ULONG result = D3D9ReleaseNext(pthis);

	if (result == 0)
	{
		if (pthis == d3d9.device.handle)
		{
			qFileLog("IUnknown::Release: The IDirect3DDevice9 has been freed.");
			d3d9.device.handle = 0;
		}
		else if (pthis == d3d9.obj.handle)
		{
			qFileLog("IUnknown::Release: The IDirect3D9 interface has been freed.");
			d3d9.obj.handle = 0;
		}
	}
	return result;
}

DOUBLE BufDif = 1.000;
HRESULT WINAPI D3D9GetRasterStatusHook(IDirect3DDevice9 *pthis, UINT iSwapChain, D3DRASTER_STATUS *pRasterStatus)
{
	//for extra-tall resolutions with Assisted Vsync enabled

	#ifdef LOGGING
	if (PIGI < 4)
	{
		PIGI++;
		fFileLog("IDirect3DDevice9::GetRasterStatus hook call %u%s", PIGI,
			PIGI >= 4 ? ". Further calls are not recorded." : ".");
	}
	#endif

	HRESULT result = D3D9GetRasterStatusNext(pthis, iSwapChain, pRasterStatus);
	pRasterStatus->ScanLine = (UINT)((DOUBLE)pRasterStatus->ScanLine * BufDif);
	return result;
}

HRESULT WINAPI D3D9PresentHook(IDirect3DDevice9 *pthis, const RECT *pSourceRect, const RECT *pDestRect,
	HWND hDestWindowOverride, const RGNDATA *pDirtyRegion)
{
	#ifdef LOGGING
	if (PIPI < 4)
	{
		PIPI++;
		fFileLog("IDirect3DDevice9::Present: hook call %u%s", PIPI,
			PIPI >= 4 ? ". Further calls are not recorded, except for some TestCooperativeLevel errors." : ".");
	}
	#endif

	if (Settings.Misc.FancyStartup) FancyUpdate();
	if (WA.Version < QV(3,7,2,17))
	{
		HRESULT cooptest = pthis->TestCooperativeLevel();
		switch (cooptest)
		{
		case D3DERR_DEVICELOST:
			//	qFileLog("TestCooperativeLevel returned D3DERR_DEVICELOST");
			Sleep(1);
			return D3D_OK;
		case D3DERR_DEVICENOTRESET:
			qFileLog("TestCooperativeLevel returned D3DERR_DEVICENOTRESET, acknowledging and preparing to recreate the device.");
			return D3DERR_DEVICELOST;
		}

		HRESULT result = D3D9PresentNext(pthis, pSourceRect, pDestRect, hDestWindowOverride, pDirtyRegion);
		
		if (result == D3DERR_DEVICELOST)
			result = -1; //prevent W:A from reinitializing device when it's not possible: we handle this
		return result;
	}
	return D3D9PresentNext(pthis, pSourceRect, pDestRect, hDestWindowOverride, pDirtyRegion);
}

HRESULT WINAPI D3D9ResetHook(IDirect3DDevice9* pthis, D3DPRESENT_PARAMETERS *pParams)
{
	qFileLog("Attempting to reset IDirect3DDevice9: hook engaged.");

	WA.BB.Width = pParams->BackBufferWidth;
	WA.BB.Height = pParams->BackBufferHeight;

	fFileLog("Expected backbuffer size: %ux%u", WA.BB.Width, WA.BB.Height);

	if (!Settings.FR.Fullscreen || Settings.FR.AltFullscreen || InGame())
		pParams->Windowed = TRUE;

	SetWndParam(pParams->hDeviceWindow, 0, 0, 0, pParams->BackBufferWidth, pParams->BackBufferHeight, SWP_SHOWWINDOW | SWP_NOREDRAW);

	HRESULT result = D3D9ResetNext(pthis, pParams);
	if (SUCCEEDED(result))
		qFileLog("Device reset successfully!");
	else
		fFileLog("Device reset FAILURE! Error: 0x%X", result);
	return result;
}

HRESULT WINAPI D3D9CreateDeviceHook(IDirect3D9 *pthis, UINT Adapter, D3DDEVTYPE DeviceType, HWND hFocusWindow, DWORD BehaviorFlags,
	D3DPRESENT_PARAMETERS *pParams, IDirect3DDevice9 **ppReturnedDeviceInterface)
{
	qFileLog("Attempting to create IDirect3DDevice9: hook engaged.");

	WA.BB.Width = pParams->BackBufferWidth;
	WA.BB.Height = pParams->BackBufferHeight;

	fFileLog("Expected backbuffer size: %ux%u", WA.BB.Width, WA.BB.Height);

	SetWindowedMode:
	if (!Settings.FR.Fullscreen || Settings.FR.AltFullscreen || InGame())
		pParams->Windowed = TRUE;

	SetWndParam(pParams->hDeviceWindow, 0, 0, 0, pParams->BackBufferWidth, pParams->BackBufferHeight, SWP_SHOWWINDOW | SWP_NOREDRAW);

	HRESULT result = D3D9CreateDeviceNext(pthis, Adapter, DeviceType, hFocusWindow, BehaviorFlags, pParams, ppReturnedDeviceInterface);
	if (SUCCEEDED(result))
	{
		qFileLog("Successfully created IDirect3DDevice9.");
		d3d9.RefreshDevice(*ppReturnedDeviceInterface);

		if (!D3D9ReleaseNext)
		{
			if (MH_CreateHook(d3d9.device.Release, D3D9ReleaseHook, (PVOID*)&D3D9ReleaseNext) == MH_OK)
			{
				qFileLog("Successfully hooked IUnknown::Release.");
				if (MH_EnableHook(d3d9.device.Release) != MH_OK)
					qFileLog("FAILED to enable the IUnknown::Release hook!");
			}
			else
				qFileLog("FAILED to hook IUnknown::Release!");
		}

		if (!D3D9ResetNext)
		{
			if (MH_CreateHook(d3d9.device.Reset, D3D9ResetHook, (PVOID*)&D3D9ResetNext) == MH_OK)
			{
				qFileLog("Successfully hooked IDirect3DDevice9::Reset.");
				if (MH_EnableHook(d3d9.device.Reset) != MH_OK)
					qFileLog("FAILED to enable the IDirect3DDevice9::Reset hook!");
			}
			else
				qFileLog("FAILED to hook IDirect3DDevice9::Reset!");
		}

		if (WA.Version < QV(3,7,2,17) || (!InGame() && Settings.Misc.FancyStartup && !Settings.FR.Fullscreen && !Settings.FR.AltFullscreen))
		{
			if (!D3D9PresentNext)
			if (MH_CreateHook(d3d9.device.Present, D3D9PresentHook, (PVOID*)&D3D9PresentNext) == MH_OK)
			{
				qFileLog("Successfully hooked IDirect3DDevice9::Present.");
				if (MH_EnableHook(d3d9.device.Present) != MH_OK)
					qFileLog("FAILED to enable the IDirect3DDevice9::Present hook!");
			}
			else
				qFileLog("FAILED to hook IDirect3DDevice9::Present!");
		}

		if (!D3D9GetRasterStatusNext && (pParams->BackBufferHeight > (UINT)Env.Sys.PrimResY || pParams->BackBufferHeight < 120))
		{
			BufDif = pParams->BackBufferHeight / (DOUBLE)Env.Sys.PrimResY;
			if (MH_CreateHook(d3d9.device.GetRasterStatus, D3D9GetRasterStatusHook, (PVOID*)&D3D9GetRasterStatusNext) == MH_OK)
			{
				qFileLog("Successfully hooked IDirect3DDevice9::GetRasterStatus, required for the extra-tall backbuffer size.");
				if (MH_EnableHook(d3d9.device.GetRasterStatus) != MH_OK)
					qFileLog("FAILED to enable the IDirect3DDevice9::GetRasterStatus hook!");
			}
			else
				qFileLog("FAILED to hook IDirect3DDevice9::GetRasterStatus, required for the extra-tall backbuffer size!");
		}
		else if (D3D9GetRasterStatusNext)
		{
			if (MH_DisableHook(d3d9.device.GetRasterStatus) == MH_OK)
			{
				qFileLog("Successfully unhooked IDirect3DDevice9::GetRasterStatus.");
				D3D9GetRasterStatusNext = 0;
			}
			else
				qFileLog("FAILED to unhook IDirect3DDevice9::GetRasterStatus!");
		}
	}
	else if (result == D3DERR_INVALIDCALL)
	{
		if (Settings.FR.Fullscreen && !Settings.FR.AltFullscreen && !InGame())
		{
			qFileLog("ERROR: D3DERR_INVALIDCALL when creating the device. Fullscreen mode is on. Checking whether the requested screen resolution is unsupported.");

			HRESULT check = d3d9.CheckFullscreenMode(Adapter, pParams);
			if (check == D3DERR_NOTAVAILABLE)
			{
				qFileLog("End of test: the requested fullscreen mode is NOT supported by the adapter. Switching to windowed mode and notifying the user.");

				WritePrivateProfileInt("FrontendSettings", "Fullscreen", Settings.FR.Fullscreen = false, Config);
				WritePrivateProfileInt("FrontendSettings", "FullscreenAlternative", Settings.FR.AltFullscreen = false, Config);
				SetWindowPos(pParams->hDeviceWindow, NULL, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOZORDER);
				M_UnsupportedFullscreen(WA.BB.Width, WA.BB.Height, DISP_CHANGE_BADMODE);
				goto SetWindowedMode;
			}
			else if (check == D3D_OK)
				qFileLog("Test passed: the requested fullscreen mode is supported. The issue is in something else. It's possible that the device cannot be created at this time. Returning error.");
			else
				fFileLog("Unknown error during CheckFullscreenMode test! Code: 0x%X", check);
		}
		else
			qFileLog("ERROR: Wild D3DERR_INVALIDCALL when creating the device. It's possible that the device cannot be created at this time. Returning error.");
	}
	else
		fFileLog("FAILURE when creating IDirect3DDevice9: 0x%X! This may lead to the game exiting. Returning error.", result);

	return result;
}

IDirect3D9* WINAPI Direct3DCreate9Hook(UINT SDKVersion)
{
	fFileLog("Attempting to create IDirect3D9: hook entered. SDKVersion: %u", SDKVersion);
	IDirect3D9* result;

	result = Direct3DCreate9Next(SDKVersion);
	if (result)
	{
		qFileLog("Successfully created IDirect3D9.");
		if (!d3d9.obj.handle)
		{
			d3d9.RefreshInterface(result);
			LoadConfig();
			InstallHooks();
		}
	}
	else
		qFileLog("FAILURE when creating IDirect3D9! This may lead to the game exiting.");
	return result;
}

BOOL InGameHandled = 0;
BOOL __stdcall SetWndParam(HWND hWnd, HWND hWndInsertAfter, int X, int Y, int cx, int cy, UINT uFlags)
{
	fFileLog("SetWndParam has been called with parameters 0x%X, 0x%X, %d, %d, %d, %d, 0x%X.", hWnd, hWndInsertAfter, X, Y, cx, cy, uFlags);

	WA.Wnd.DX = hWnd;

	if (InGame())
	{
		qFileLog("SetWndParam: Preparing to handle in-game.");

		if (!InGameHandled)
		{
			InGameHandled = true;
			Settings.Misc.FancyStartup = false;

			if (Settings.IG.WindowBorder && !Settings.IG.Stretch && WA.BB.Width < Env.Act.ResX && WA.BB.Height < Env.Act.ResY)
			{
				RECT ClRect = { X, Y, cx, cy };
				DWORD Style = GetWindowLong(hWnd, GWL_STYLE) | WS_CAPTION;
				AdjustWindowRect(&ClRect, Style, 0);
				cx = ClRect.right - ClRect.left;
				cy = ClRect.bottom - ClRect.top;

				if (cx <= Env.Act.ResX || cy <= Env.Act.ResY)
				{
					SetWindowLong(hWnd, GWL_STYLE, Style &~WS_MAXIMIZEBOX);
					fFileLog("SetWndParam: Window border was enabled, the related checks have been done, and rect adjusted. New window size: %dx%d.", cx, cy);
				}
				else
				{
					Settings.IG.WindowBorder = 0;
					SetWindowLong(hWnd, GWL_STYLE, GetWindowLong(hWnd, GWL_STYLE) &~WS_CAPTION); //If the new frontend has a border
					qFileLog("SetWndParam: Window border was enabled, but it's too big for the requested window size/mode. It has now been disabled.");
				}
					
			}
			else
			{
				SetWindowLong(hWnd, GWL_STYLE, GetWindowLong(hWnd, GWL_STYLE) &~WS_CAPTION); //If the new frontend has a border
				qFileLog("SetWndParam: Window border is either disabled or is too big for the requested window size/mode.");
			}

			qFileLog("SetWndParam: Entering in-game and adjusting everything that we need.");

			X = Env.Act.VirtX;
			Y = Env.Act.VirtY;
			cx = MaxCap(cx, Env.Act.ResX);
			cy = MaxCap(cy, Env.Act.ResY);

			if (Settings.FR.AltFullscreen)
			{
				LONG dispChange = SetScreenRes(Env.Sys.PrimResX, Env.Sys.PrimResY);
				if (dispChange == DISP_CHANGE_SUCCESSFUL)
					fFileLog("SetWndParam: Successfully restored the primary monitor's initial resolution (%ux%u).", Env.Sys.PrimResX, Env.Sys.PrimResY);
				else
					fFileLog("SetWndParam: FAILED to restore the primary monitor's initial resolution (%ux%u)! Error: %s", Env.Sys.PrimResX, Env.Sys.PrimResY, DispChangeErrorStrA(dispChange));
			}

			ShowWindow(WA.Wnd.MFC, SW_HIDE);
			SetWindowPos(WA.Wnd.MFC, NULL, 0, 0, 0, 0, SWP_NOZORDER | SWP_NOMOVE);
			EnableWindow(hWnd, 1);

			qFileLog("SetWndParam: Last MFC overlay has been hidden and made 0x0 for the game session to prevent issues. W:A window has been enabled.");

			ClipCursor(NULL);

			qFileLog("SetWndParam: Removed ClipCursor if it existed.");

			if (WA.BB.Width < 143 && WA.BB.Width >= 0 && WA.BB.Height >= 0)
			{
				fFileLog("SetWndParam: Width (%d) is less than 143. That's too small for W:A, notifying the user and DESTROYING THE WINDOW!", cx);
				M_TooSmallWidth(WA.Wnd.DX);
				SendMessage(hWnd, WM_CLOSE, 0, 0);
				return 1;
			}

			if (Settings.IG.Background && WA.Version < QV(3,7,2,47))
			{
				qFileLog("SetWndParam: Active background is enabled: setting the W:A window as parent to the Worms2D window.");
				SetParent(WA.Wnd.W2D, WA.Wnd.DX);
			}

			UpdateWACaption();

			if (Settings.IG.Stretch)
			{
				fFileLog("SetWndParam: In-game stretching is enabled. Setting the window size to the overall resolution (%dx%d).", Env.Act.ResX, Env.Act.ResY);

				cx = Env.Act.ResX;
				cy = Env.Act.ResY;
			}

			else if (!Settings.IG.TopLeftPosition)
			{
				X += Env.Act.ResX / 2 - cx / 2;
				Y += Env.Act.ResY / 2 - cy / 2;

				qFileLog("SetWndParam: Centered the target window.");
			}	
		}
		else
		{
			uFlags |= SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER;
			qFileLog("SetWndParam: We have already been in-game: so, not adjusting anything.");
		}
	}
	else
	{
		if (InGameHandled || !Env.FrontInit)
		{
			Env.FrontInit = true;
			qFileLog("SetWndParam: Entering frontend and adjusting everything we need.");

			if (Settings.Misc.FancyStartup && !Settings.FR.AltFullscreen && !Settings.FR.Fullscreen)
				FancyUpdate();

			if (InGameHandled)
			{
				qFileLog("SetWndParam: This is a return to frontend after the match.");
				if (IsWindow(WA.Wnd.MFC) && !IsNullRect(&WA.Rect.MFC))
				{
					SetWindowPos(WA.Wnd.MFC, NULL, WA.Rect.MFC.left, WA.Rect.MFC.top, WA.Rect.MFC.right - WA.Rect.MFC.left, WA.Rect.MFC.bottom - WA.Rect.MFC.top, SWP_NOMOVE | SWP_NOZORDER);
					ShowWindow(WA.Wnd.MFC, SW_SHOW);
					qFileLog("SetWndParam: Restored the last MFC overlay to its initial size and display.");
				}

				if (Settings.IG.WindowBorder && !(WA.Version >= QV(3, 7, 2, 46) && Settings.FR.Centered))
				{
					qFileLog("SetWndParam: trying to remove the window border manually before it's done in an unsafe automatic way. [test]");
					SetWindowLong(hWnd, GWL_STYLE, GetWindowLong(hWnd, GWL_STYLE) &~WS_CAPTION &~WS_MAXIMIZEBOX);
				}
				InGameHandled = false;
			}

			if (Settings.FR.AltFullscreen)
			{
				qFileLog("SetWndParam: User is running artificial fullscreen. Preparing to change resolution via ChangeDisplaySettings.");

				hWndInsertAfter = HWND_TOPMOST;
				LONG dispChange = 0;
				if ((dispChange = SetScreenRes(WA.BB.Width, WA.BB.Height)) != DISP_CHANGE_SUCCESSFUL)
				{
					fFileLog("SetWndParam: FAILED to set the primary monitor's resolution to %ux%u for frontend in artificial fullscreen mode!"
						"Error: %s. Switching to windowed mode and prompting the user.", WA.BB.Width, WA.BB.Height, DispChangeErrorStrA(dispChange));

					WritePrivateProfileInt("FrontendSettings", "Fullscreen", Settings.FR.Fullscreen = false, Config);
					WritePrivateProfileInt("FrontendSettings", "FullscreenAlternative", Settings.FR.AltFullscreen = false, Config);
					SetWindowText(WA.Wnd.DX, "Worms Armageddon (windowed)");
					M_UnsupportedFullscreen(WA.BB.Width, WA.BB.Height, dispChange);
				}
				else
				{
					SetWindowText(WA.Wnd.DX, "Worms Armageddon");
					fFileLog("SetWndParam: Successfully set the primary monitor's resolution to %ux%u for frontend in artificial fullscreen mode. Calling SetWindowPos.", WA.BB.Width, WA.BB.Height);
				}
			}
			else if (Settings.FR.Fullscreen)
			{
				SetWindowText(WA.Wnd.DX, "Worms Armageddon");

				qFileLog("SetWndParam: Returning. We are in fullscreen frontend mode: no further adjustments required.");
				return TRUE;
			}
			else
			{
				SetWindowText(WA.Wnd.DX, "Worms Armageddon (windowed)");

				if (Settings.FR.Stretch)
				{
					if (!Settings.MM.Enable)
						ClipCursorInFrontend();

					X = 0;
					Y = 0;
					cx = Env.Sys.PrimResX;
					cy = Env.Sys.PrimResY;

					qFileLog("SetWndParam: Calling next hook. Stretch mode.");
				}

				else if (Settings.FR.Centered)
				{
					if (Settings.FR.ArbitrarySizing)
					{
						ClipCursorInFrontend();
						X = MinCap((Env.Sys.PrimResX / 2) - (Settings.FR.Xsize / 2), 0);
						Y = MinCap((Env.Sys.PrimResY / 2) - (Settings.FR.Ysize / 2), 0);
						cx = MaxCap(Settings.FR.Xsize, Env.Sys.PrimResX);
						cy = MaxCap(Settings.FR.Ysize, Env.Sys.PrimResY);

						qFileLog("SetWndParam: Calculated the position of CenteredFrontend with CustomSize enabled.");
					}

					else
					{
						if (WA.Version < QV(3, 7, 2, 46))
						{
							ClipCursorInFrontend();
							X = MinCap((Env.Sys.PrimResX / 2) - (WA.BB.Width / 2), 0);
							Y = MinCap((Env.Sys.PrimResY / 2) - (WA.BB.Height / 2), 0);
						}
						else
						{
							RECT ClRect = { X, Y, cx, cy };
							DWORD Style = GetWindowLong(hWnd, GWL_STYLE) | WS_CAPTION;
							AdjustWindowRect(&ClRect, Style, 0);
							cx = ClRect.right - ClRect.left;
							cy = ClRect.bottom - ClRect.top;
							SetWindowLong(hWnd, GWL_STYLE, Style &~WS_MAXIMIZEBOX);
							X = MinCap((Env.Sys.PrimResX / 2) - (WA.BB.Width / 2), 0);
							Y = MinCap((Env.Sys.PrimResY / 2) - (WA.BB.Height / 2), 0);
						}

						qFileLog("SetWndParam: Calculated the position of CenteredFrontend.");
					}

				}

				else if (Settings.FR.ArbitrarySizing)
				{
					ClipCursorInFrontend();
					cx = MaxCap(Settings.FR.Xsize, Env.Sys.PrimResX);
					cy = MaxCap(Settings.FR.Ysize, Env.Sys.PrimResY);

					qFileLog("SetWndParam: Calling next hook. Calculated the position with CustomSize enabled.");
				}
			}
		}
		else
		{
			uFlags |= SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER;
			qFileLog("SetWndParam: Frontend already adjusted; ignoring.");
		}
	}

	if (Settings.Misc.NoTopmost) hWndInsertAfter = HWND_NOTOPMOST;
	else hWndInsertAfter = HWND_TOPMOST;

	fFileLog("SetWndParam: Window will be set as %s.", Settings.Misc.NoTopmost ? "NOTOPMOST" : "TOPMOST");

	qFileLog("SetWndParam: Calling SetWindowPos.");
	return SetWindowPos(hWnd, hWndInsertAfter, X, Y, cx, cy, uFlags);
}

HRESULT WINAPI DSoundCreateSoundBufferHook(IDirectSound* pthis, LPDSBUFFERDESC pcDSBufferDesc,
	LPDIRECTSOUNDBUFFER * ppDSBuffer, LPUNKNOWN pUnkOuter)
{
//	fFileLog("IDirectSound::CreateSoundBuffer hook engaged. dwFlags: 0x%X. Adding DSBCAPS_GLOBALFOCUS for background sound playback.", pcDSBufferDesc->dwFlags);
	if (!!!(pcDSBufferDesc->dwFlags & DSBCAPS_PRIMARYBUFFER))
		pcDSBufferDesc->dwFlags |= DSBCAPS_GLOBALFOCUS;
	return DSoundCreateSoundBufferNext(pthis, pcDSBufferDesc, ppDSBuffer, pUnkOuter);
}

HRESULT WINAPI DSoundCreateHook(LPCGUID lpcGuidDevice, LPDIRECTSOUND *ppDS, LPUNKNOWN pUnkOuter)
{
	fFileLog("DirectSoundCreate hook engaged. LPGUID: 0x%X, ppDS: 0x%X, pUnkOuter: 0x%X", lpcGuidDevice, ppDS, pUnkOuter);
	HRESULT result = DSoundCreateNext(lpcGuidDevice, ppDS, pUnkOuter);
	if (SUCCEEDED(result) && !dsound.obj)
	{
		dsound.obj = *ppDS;
		if (MH_CreateHook(VMTEntry(dsound.obj, __CreateSoundBuffer), DSoundCreateSoundBufferHook, (PVOID*)&DSoundCreateSoundBufferNext) != MH_OK)
			qFileLog("FAILED to hook IDirectSound::CreateSoundBuffer!");
		else
		{
			qFileLog("Successfully hooked IDirectSound::CreateSoundBuffer.");
			if (MH_EnableHook(VMTEntry(dsound.obj, __CreateSoundBuffer)) != MH_OK)
				qFileLog("FAILED to enable the IDirectSound::CreateSoundBuffer hook!");
		}
	}
	return result;
}
