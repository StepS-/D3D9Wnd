#pragma once

#include <Windows.h>

BOOL InitializeD3D9Wnd();
BOOL FinalizeD3D9Wnd();
void LoadConfig();
void WriteRunInBackground();
void CheckStaticWindowClass();
void GetWALocale();
BOOL __stdcall InstallHooks();
void __stdcall UninstallHooks();
