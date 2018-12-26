
#include <Windows.h>
#include <d3d9.h>
#include "MinHook\MinHook.h"
#include "tools\tools.h"

HMODULE d3d9;
IDirect3D9* (WINAPI *Direct3DCreate9Next)(UINT);
HRESULT(WINAPI *D3D9CreateDeviceNext)(IDirect3D9*, UINT, D3DDEVTYPE, HWND, DWORD, D3DPRESENT_PARAMETERS*, IDirect3DDevice9**);
HRESULT(WINAPI *D3D9ResetNext)(IDirect3DDevice9*, D3DPRESENT_PARAMETERS*);

HRESULT WINAPI D3D9ResetHook(IDirect3DDevice9* pthis, D3DPRESENT_PARAMETERS *pParams)
{
	pParams->Windowed = TRUE;
	HRESULT result = D3D9ResetNext(pthis, pParams);
	if (SUCCEEDED(result))
		SetWindowPos(pParams->hDeviceWindow, 0, 0, 0, pParams->BackBufferWidth, pParams->BackBufferHeight, SWP_SHOWWINDOW | SWP_NOREDRAW | SWP_NOZORDER);
	return result;
}

HRESULT WINAPI D3D9CreateDeviceHook(IDirect3D9 *pthis, UINT Adapter, D3DDEVTYPE DeviceType, HWND hFocusWindow, DWORD BehaviorFlags,
	D3DPRESENT_PARAMETERS *pParams, IDirect3DDevice9 **ppReturnedDeviceInterface)
{
	pParams->Windowed = TRUE;
	HRESULT result = D3D9CreateDeviceNext(pthis, Adapter, DeviceType, hFocusWindow, BehaviorFlags, pParams, ppReturnedDeviceInterface);
	if (SUCCEEDED(result))
	{
		SetWindowPos(pParams->hDeviceWindow, 0, 0, 0, pParams->BackBufferWidth, pParams->BackBufferHeight, SWP_SHOWWINDOW | SWP_NOREDRAW | SWP_NOZORDER);
		if (!D3D9ResetNext)
		{
			if (MH_CreateHook(VMTEntry(*ppReturnedDeviceInterface, 16), D3D9ResetHook, (PVOID*)&D3D9ResetNext) == MH_OK)
				MH_EnableHook(VMTEntry(*ppReturnedDeviceInterface, 16));
		}
	}
	return result;
}

IDirect3D9* WINAPI Direct3DCreate9Hook(UINT SDKVersion)
{
	IDirect3D9* result = Direct3DCreate9Next(SDKVersion);
	if (result && !D3D9CreateDeviceNext)
	{
		if (MH_CreateHook(VMTEntry(result, 16), D3D9CreateDeviceHook, (PVOID*)&D3D9CreateDeviceNext) == MH_OK)
			MH_EnableHook(VMTEntry(result, 16));
	}
	return result;
}

BOOL APIENTRY DllMain(HMODULE, DWORD ul_reason_for_call, LPVOID)
{
	if (ul_reason_for_call == DLL_PROCESS_ATTACH)
	{
//		if (!LockCurrentInstance("D3D9Wnd")) //allow only one module within the same process
//			return 1;
		if ((d3d9 = LoadLibrary("d3d9.dll")) != 0)
		{
			FARPROC D3D9Create = GetProcAddress(d3d9, "Direct3DCreate9");
			if (D3D9Create && MH_Initialize() == MH_OK)
			{
				if (MH_CreateHook(D3D9Create, Direct3DCreate9Hook, (PVOID*)&Direct3DCreate9Next) == MH_OK)
					MH_EnableHook(D3D9Create);
			}
		}
	}

	else if (ul_reason_for_call == DLL_PROCESS_DETACH)
	{
		if (d3d9)
		{
			MH_DisableHook(MH_ALL_HOOKS);
			FreeLibrary(d3d9);
			d3d9 = 0;
		}
	}

	return true;
}
