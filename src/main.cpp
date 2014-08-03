
#include <Windows.h>
#include "init.h"

BOOL APIENTRY DllMain(HMODULE, DWORD ul_reason_for_call, LPVOID)
{
	if (ul_reason_for_call == DLL_PROCESS_ATTACH)
		return InitializeD3D9Wnd();

	else if (ul_reason_for_call == DLL_PROCESS_DETACH)
		return FinalizeD3D9Wnd();

	return TRUE;
}
