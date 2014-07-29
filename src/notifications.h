#pragma once
#include <Windows.h>
#include <stdio.h>

extern char walanguage[32];
#define GameLanguage(LangString) (!strcmp((char*)walanguage, LangString) ? true : false)
#define RegistryLanguage GameLanguage

void M_TooSmallWidth(HWND);
void M_WndmodeDetected();
void M_D3D9Disabled();
void M_TooOld();
void M_HardwareCursorsDisabled();
void M_RecommendHardwareCursorsDisable();
void M_SetMultiMonResLight(int, int);
void M_LowBPP(UINT);
void M_UnsupportedFullscreen(int, int, LONG);
