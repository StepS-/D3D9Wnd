
#include <Windows.h>
#include "init.h"

BOOL APIENTRY DllMain(HMODULE, DWORD fdwReason, LPVOID)
{
	switch (fdwReason)
	{
	case DLL_PROCESS_ATTACH: return InitializeD3D9Wnd();
	case DLL_PROCESS_DETACH: return FinalizeD3D9Wnd();
	}
	return TRUE;
}
