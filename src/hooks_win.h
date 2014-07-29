#pragma once

#include <Windows.h>

extern HHOOK hkb, hkb2, hkb3;
LRESULT CALLBACK KeyboardProc(int nCode, WPARAM wKeyCode, LPARAM lParam);
LRESULT CALLBACK CallWndProc(int nCode, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK MouseProc(int nCode, WPARAM wParam, LPARAM lParam);
