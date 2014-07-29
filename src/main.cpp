
#include <Windows.h>
#include "init.h"

#ifdef STATICHOOKS
#pragma comment (lib, "MinHook\\libMinHook.x86.lib")
#else
#pragma comment (lib, "MinHook\\MinHook.x86.lib")
#endif

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{
	if (ul_reason_for_call == DLL_PROCESS_ATTACH)
		return InitializeD3D9Wnd();

	else if (ul_reason_for_call == DLL_PROCESS_DETACH)
		return FinalizeD3D9Wnd();

	return TRUE;
}
