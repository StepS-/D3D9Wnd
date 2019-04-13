//Worms development tools by StepS

#pragma once

#include <Windows.h>
#include <stdio.h>

#ifdef VISTAUP
#include <dwmapi.h>
#endif

#pragma comment (lib, "version.lib")
typedef unsigned long long QWORD;
typedef QWORD *PQWORD, *LPQWORD;

extern "C" IMAGE_DOS_HEADER __ImageBase;
#define HINST_THISCOMPONENT ((HINSTANCE)&__ImageBase)

#define IJ_JUMP 0 //Insert a jump (0xE9) with InsertJump
#define IJ_CALL 1 //Insert a call (0xE8) with InsertJump
#define IJ_FARJUMP 2 //Insert a farjump (0xEA) with InsertJump
#define IJ_FARCALL 3 //Insert a farcall (0x9A) with InsertJump
#define IJ_PUSHRET 4 //Insert a pushret with InsertJump

#define AlignInteger(num, mod) (num + (mod * ((num % mod) != 0) - (num % mod)))
#define MAKELONGLONG(lo,hi) ((LONGLONG(DWORD(lo) & 0xffffffff)) | LONGLONG(DWORD(hi) & 0xffffffff) << 32 )
#define LineGetSubPos(SBig, SSmall) ((SBig - SSmall)/2)
#define MAKEQWORD(LO2, HI2, LO1, HI1) MAKELONGLONG(MAKELONG(LO2,HI2),MAKELONG(LO1,HI1))
#define KeyPressed(k) (GetAsyncKeyState(k) < 0)
#define EnforceRange(val, v_min, v_max) (val < v_min ? v_min : val > v_max ? v_max : val)
#define MinCap(nSize, nMin) (nSize < nMin ? nMin : nSize)
#define MaxCap(nSize, nMax) (nSize > nMax ? nMax : nSize)
#define ModularDifference(a, b) (a > b ? a - b : b - a) //for both signed and unsigned
#define GetExeVersion() GetModuleVersion((HMODULE)0)
#define GetWAVersion GetExeVersion
#define ShowCursorCount GetCursorCounter
#define ShowCursorN SetCursorCounter
#define QV(V1, V2, V3, V4) MAKEQWORD(V4, V3, V2, V1)

#define VMTEntry(ObjPtr, EntryNum) (*(PVOID*)(*(PULONG_PTR)ObjPtr + EntryNum*sizeof(ULONG_PTR)))

#define GenericBoxA(Text, Caption, Type) MessageBoxA(0, Text, Caption, Type)
#define GenericBoxW(Text, Caption, Type) MessageBoxW(0, Text, Caption, Type)
#define InfoBoxA(Text, Caption) GenericBoxA(Text, Caption, MB_OK | MB_ICONINFORMATION)
#define InfoBoxW(Text, Caption) GenericBoxW(Text, Caption, MB_OK | MB_ICONINFORMATION)
#define ErrorBoxA(Text, Caption) GenericBoxA(Text, Caption, MB_OK | MB_ICONERROR)
#define ErrorBoxW(Text, Caption) GenericBoxW(Text, Caption, MB_OK | MB_ICONERROR)
#define WarningBoxA(Text, Caption) GenericBoxA(Text, Caption, MB_OK | MB_ICONWARNING)
#define WarningBoxW(Text, Caption) GenericBoxW(Text, Caption, MB_OK | MB_ICONWARNING)

int MprintfA(UINT uType, LPCSTR lpCaption, LPCSTR Format, ...);
int MprintfW(UINT uType, LPCWSTR lpCaption, LPCWSTR Format, ...);

#define MprintfiA(Caption, Format, ...) MprintfA(MB_OK | MB_ICONINFORMATION, Caption, Format, __VA_ARGS__)
#define MprintfiW(Caption, Format, ...) MprintfW(MB_OK | MB_ICONINFORMATION, Caption, Format, __VA_ARGS__)
#define MprintfeA(Caption, Format, ...) MprintfA(MB_OK | MB_ICONERROR, Caption, Format, __VA_ARGS__)
#define MprintfeW(Caption, Format, ...) MprintfW(MB_OK | MB_ICONERROR, Caption, Format, __VA_ARGS__)
#define MprintfwA(Caption, Format, ...) MprintfA(MB_OK | MB_ICONWARNING, Caption, Format, __VA_ARGS__)
#define MprintfwW(Caption, Format, ...) MprintfW(MB_OK | MB_ICONWARNING, Caption, Format, __VA_ARGS__)

#define GetWARegOptionStringA(OptionName, OutBuf, BufSize, Default) GetRegistryStringA(HKEY_CURRENT_USER, "Software\\Team17SoftwareLTD\\WormsArmageddon\\Options\\", OptionName, OutBuf, BufSize, Default)
#define GetWARegOptionStringW(OptionName, OutBuf, BufSize, Default) GetRegistryStringW(HKEY_CURRENT_USER, L"Software\\Team17SoftwareLTD\\WormsArmageddon\\Options\\", OptionName, OutBuf, BufSize, Default)
#define GetWARegOptionDwordA(OptionName, Default) GetRegistryDwordA(HKEY_CURRENT_USER, "Software\\Team17SoftwareLTD\\WormsArmageddon\\Options\\", OptionName, Default)
#define GetWARegOptionDwordW(OptionName, Default) GetRegistryDwordW(HKEY_CURRENT_USER, L"Software\\Team17SoftwareLTD\\WormsArmageddon\\Options\\", OptionName, Default)
#define GetWARegPathA(OutBuf, BufSize, Default) GetRegistryStringA(HKEY_CURRENT_USER, "Software\\Team17SoftwareLTD\\WormsArmageddon\\", "PATH", OutBuf, BufSize, Default)
#define GetWARegPathW(OutBuf, BufSize, Default) GetRegistryStringW(HKEY_CURRENT_USER, L"Software\\Team17SoftwareLTD\\WormsArmageddon\\", "PATH", OutBuf, BufSize, Default)

#define WriteWARegOptionDwordA(OptionName, NewValue) WriteRegistryDwordA(HKEY_CURRENT_USER, "Software\\Team17SoftwareLTD\\WormsArmageddon\\Options\\", OptionName, NewValue)
#define WriteWARegOptionDwordW(OptionName, NewValue) WriteRegistryDwordW(HKEY_CURRENT_USER, L"Software\\Team17SoftwareLTD\\WormsArmageddon\\Options\\", OptionName, NewValue)

#ifdef UNICODE
#define GetPathUnderModule GetPathUnderModuleW
#define GetRegistryString GetRegistryStringW
#define GetRegistryDword GetRegistryDwordW
#define GetWARegOptionString GetWARegOptionStringW
#define GetWARegOptionDword GetWARegOptionDwordW
#define GetWARegPath GetWARegPathW
#define WritePrivateProfileInt WritePrivateProfileIntW
#define GetPrivateProfileDouble GetPrivateProfileDoubleW
#define WriteRegistryDword WriteRegistryDwordW
#define WriteWARegOptionDword WriteWARegOptionDwordW
#define GenericBox GenericBoxW
#define InfoBox InfoBoxW
#define ErrorBox ErrorBoxW
#define WarningBox WarningBoxW
#define PatchMemString PatchMemStringW
#define DispChangeErrorStr DispChangeErrorStrW
#define FileExists FileExistsW
#define DirectoryExists DirectoryExistsW
#define Mprintf MprintfW
#define Mprintfi MprintfiW
#define Mprintfe MprintfeW
#define Mprintfw MprintfwW

#else
#define GetPathUnderModule GetPathUnderModuleA
#define GetRegistryString GetRegistryStringA
#define GetRegistryDword GetRegistryDwordA
#define GetWARegOptionString GetWARegOptionStringA
#define GetWARegOptionDword GetWARegOptionDwordA
#define GetWARegPath GetWARegPathA
#define WritePrivateProfileInt WritePrivateProfileIntA
#define GetPrivateProfileDouble GetPrivateProfileDoubleA
#define WriteRegistryDword WriteRegistryDwordA
#define WriteWARegOptionDword WriteWARegOptionDwordA
#define GenericBox GenericBoxA
#define InfoBox InfoBoxA
#define ErrorBox ErrorBoxA
#define WarningBox WarningBoxA
#define PatchMemString PatchMemStringA
#define DispChangeErrorStr DispChangeErrorStrA
#define FileExists FileExistsA
#define DirectoryExists DirectoryExistsA
#define Mprintf MprintfA
#define Mprintfi MprintfiA
#define Mprintfe MprintfeA
#define Mprintfw MprintfwA
#endif

QWORD GetModuleVersion(HMODULE hModule = NULL);
BOOL DWMEnabled();
int MakeProcessDPIAware(BOOL PerMonitor = TRUE);

BOOL WritePrivateProfileIntA(LPCSTR, LPCSTR, int, LPCSTR);
BOOL WritePrivateProfileIntW(LPCWSTR, LPCWSTR, int, LPCWSTR);
DOUBLE GetPrivateProfileDoubleA(LPCSTR, LPCSTR, DOUBLE, LPCSTR);
DOUBLE GetPrivateProfileDoubleW(LPCWSTR, LPCWSTR, DOUBLE, LPCWSTR);
LPSTR GetPathUnderModuleA(HMODULE, CHAR[MAX_PATH], LPCSTR);
LPWSTR GetPathUnderModuleW(HMODULE, WCHAR[MAX_PATH], LPCWSTR);
LSTATUS GetRegistryStringA(HKEY, LPCSTR, LPCSTR, LPSTR, DWORD, LPCSTR);
LSTATUS GetRegistryStringW(HKEY, LPCSTR, LPCSTR, LPSTR, DWORD, LPCSTR);
DWORD GetRegistryDwordA(HKEY, LPCSTR, LPCSTR, DWORD);
DWORD GetRegistryDwordW(HKEY, LPCWSTR, LPCWSTR, DWORD);
LSTATUS WriteRegistryDwordA(HKEY, LPCSTR, LPCSTR, DWORD);
LSTATUS WriteRegistryDwordW(HKEY, LPCWSTR, LPCWSTR, DWORD);
BOOL FileExistsA(LPCSTR);
BOOL FileExistsW(LPCWSTR);
BOOL DirectoryExistsA(LPCSTR);
BOOL DirectoryExistsW(LPCWSTR);

QWORD GetFileSizeQ(HANDLE hFile);

BOOL __stdcall PatchMemData(PVOID pAddr, size_t buf_len, PVOID pNewData, size_t data_len);
BOOL __stdcall PatchMemStringA(PVOID pAddr, size_t dest_len, LPSTR lpString);
BOOL __stdcall PatchMemStringW(PVOID pAddr, size_t dest_len, LPWSTR lpString);

template<typename VT>
BOOL __stdcall PatchMemVal(PVOID pAddr, VT newValue)
{return PatchMemData(pAddr, sizeof(VT), &newValue, sizeof(VT));}
template<typename VT>
BOOL __stdcall PatchMemVal(ULONG_PTR pAddr, VT newValue)
{return PatchMemData((PVOID)pAddr, sizeof(VT), &newValue, sizeof(VT));}

template<typename VT, size_t _Size>
VT RoundToNearest(VT val, VT (&arr)[_Size]/*, int direction = 0*/)
{
	VT lv = arr[0];
	for (VT z : arr)
	{
		if (z == val) return val;
			if (ModularDifference(z, val) < ModularDifference(lv, val))
				lv = z;
	}
	return lv;
}

#define SteamCheck WASteamCheck
bool WASteamCheck();

bool DataPatternCompare(LPCBYTE, LPCBYTE, LPCSTR);
ULONG_PTR FindPattern(PBYTE bMask, LPSTR szMask, ULONG_PTR dwAddress, size_t dwLen, BOOL backwards = FALSE);
ULONG_PTR FindPatternPrecise(PBYTE bMask, size_t dwCount, ULONG_PTR dwAddress, size_t dwLen, BOOL backwards = FALSE);

bool LockCurrentInstance(LPCTSTR);
bool LockGlobalInstance(LPCTSTR);

int GetCursorCounter();
void SetCursorCounter(int value);

LONG SetScreenRes(int cx, int cy);
BOOL SetWindowTransparencyLevel(HWND hWnd, BYTE bLevel);
BOOL IsNullRect(LPRECT lpRect);
BOOL RectCat(LPRECT Dest, LPRECT Source);

BOOL UnadjustWindowRectEx(LPRECT lpRect, DWORD dwStyle, BOOL fMenu, DWORD dwExStyle);
BOOL UnadjustWindowRect(LPRECT lpRect, DWORD dwStyle, BOOL fMenu);
BOOL AdjustPosViaRectEx(int& x, int& y, int& cx, int& cy, DWORD dwStyle, BOOL fMenu, DWORD dwExStyle);
BOOL AdjustPosViaRect(int& x, int& y, int& cx, int& cy, DWORD dwStyle, BOOL fMenu);
BOOL UnadjustPosViaRectEx(int& x, int& y, int& cx, int& cy, DWORD dwStyle, BOOL fMenu, DWORD dwExStyle);
BOOL UnadjustPosViaRect(int& x, int& y, int& cx, int& cy, DWORD dwStyle, BOOL fMenu);

void PosToRect(RECT &lpRect, int x, int y, int cx, int cy);
void RectToPos(RECT &lpRect, int &x, int &y, int &cx, int &cy);

LPCSTR DispChangeErrorStrA(LONG ErrorCode);
LPCWSTR DispChangeErrorStrW(LONG ErrorCode);

int LogToFileA(HANDLE hFile, LPCSTR Format, ...);
BOOL CmdOption(LPCSTR lpCmdOption);
char *_strtrim(char *lpStr);

#ifndef _M_X64
//WARN: Thread safety
BOOL __stdcall InsertJump(PVOID pDest, size_t dwPatchSize, PVOID pCallee, DWORD dwJumpType = IJ_JUMP);
#endif

typedef struct PEInfo
{
	PEInfo(HMODULE hModule = 0);
	~PEInfo() {};

	void Reset(HMODULE hModule);
	ULONG_PTR Offset(size_t off);
	BOOL PtrInCode(PVOID ptr);
	BOOL PtrInData(PVOID ptr);
	
	HANDLE Handle;
	IMAGE_DOS_HEADER* DOS;
	IMAGE_NT_HEADERS* NT;
	IMAGE_FILE_HEADER* FH;
	IMAGE_OPTIONAL_HEADER* OPT;
} *PPEInfo;
