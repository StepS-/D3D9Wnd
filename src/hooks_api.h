#pragma once

#include <Windows.h>

HWND WINAPI CreateWindowExANew(DWORD dwExStyle, LPCTSTR lpClassName, LPCTSTR lpWindowName, DWORD dwStyle, int x, int y,
	int nWidth, int nHeight, HWND hWndParent, HMENU hMenu, HINSTANCE hInstance, LPVOID lpParam);
BOOL WINAPI SetForegroundWindowNew(HWND hWnd);
BOOL WINAPI IsIconicNew(HWND hWnd);
BOOL WINAPI SetCursorPosNew(int X, int Y);
BOOL WINAPI MoveWindowNew(HWND hWnd, int X, int Y, int nWidth, int nHeight, BOOL bRepaint);
//BOOL WINAPI GetCursorPosNew(LPPOINT lpPoint);

extern HWND(WINAPI *CreateDialogIndirectParamANext)(HINSTANCE, LPCDLGTEMPLATEA, HWND, DLGPROC, LPARAM);
extern HWND(WINAPI *CreateWindowExANext)(DWORD, LPCTSTR, LPCTSTR, DWORD, int, int, int, int, HWND, HMENU, HINSTANCE, LPVOID);
extern BOOL(WINAPI *SetForegroundWindowNext)(HWND);
extern BOOL(WINAPI *IsIconicNext)(HWND);
extern BOOL(WINAPI *SetCursorPosNext)(int, int);
extern BOOL(WINAPI *MoveWindowNext)(HWND, int, int, int, int, BOOL);
//extern BOOL(WINAPI *GetCursorPosNext)(LPPOINT);
