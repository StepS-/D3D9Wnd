
#pragma once

#include <Windows.h>
#include <stdio.h>

#pragma comment (lib, "version.lib")
typedef unsigned long long QWORD;
typedef QWORD *PQWORD, *LPQWORD;

extern "C" IMAGE_DOS_HEADER __ImageBase;
#define HINST_THISCOMPONENT ((HINSTANCE)&__ImageBase)

#define MAKELONGLONG(a,b) ((LONGLONG(DWORD(a) & 0xffffffff) << 32 ) | LONGLONG(DWORD(b) & 0xffffffff))
#define GetStartPos(SBig, SSmall) (SBig/2 - SSmall/2)
#define MAKEQWORD(HI1, LO1, HI2, LO2) MAKELONGLONG(MAKELONG(HI1,LO1),MAKELONG(HI2,LO2))
#define KeyPressed(k) (!!(GetAsyncKeyState(k) & 0x8000))
#define MinCap(nSize, nMin) (nSize < nMin ? nMin : nSize)
#define MaxCap(nSize, nMax) (nSize > nMax ? nMax : nSize)
#define GetWAVersion GetFileVersionQ
#define ShowCursorCount GetCursorCounter
#define ShowCursorN SetCursorCounter
#define QV MAKEQWORD

#define VMTEntry(ObjPtr, EntryNum) (*(PVOID*)(*(PDWORD)ObjPtr + EntryNum*sizeof(ULONG)))

#define GenericBoxA(Text, Caption, Type) MessageBoxA(0, Text, Caption, Type)
#define GenericBoxW(Text, Caption, Type) MessageBoxW(0, Text, Caption, Type)
#define InfoBoxA(Text, Caption) GenericBoxA(Text, Caption, MB_OK | MB_ICONINFORMATION)
#define InfoBoxW(Text, Caption) GenericBoxW(Text, Caption, MB_OK | MB_ICONINFORMATION)
#define ErrorBoxA(Text, Caption) GenericBoxA(Text, Caption, MB_OK | MB_ICONERROR)
#define ErrorBoxW(Text, Caption) GenericBoxW(Text, Caption, MB_OK | MB_ICONERROR)
#define WarningBoxA(Text, Caption) GenericBoxA(Text, Caption, MB_OK | MB_ICONWARNING)
#define WarningBoxW(Text, Caption) GenericBoxW(Text, Caption, MB_OK | MB_ICONWARNING)

#define Mprintfq(Format, ...) Mprintf(MB_OK | MB_ICONINFORMATION, "Information", Format, __VA_ARGS__)
#define Mprintfi(Caption, Format, ...) Mprintf(MB_OK | MB_ICONINFORMATION, Caption, Format, __VA_ARGS__)
#define Mprintfe(Caption, Format, ...) Mprintf(MB_OK | MB_ICONERROR, Caption, Format, __VA_ARGS__)
#define Mprintfw(Caption, Format, ...) Mprintf(MB_OK | MB_ICONWARNING, Caption, Format, __VA_ARGS__)

#define GetWARegOptionStringA(OptionName, OutBuf, BufSize, Default) GetRegistryStringA(HKEY_CURRENT_USER, "Software\\Team17SoftwareLTD\\WormsArmageddon\\Options\\", OptionName, OutBuf, BufSize, Default)
#define GetWARegOptionStringW(OptionName, OutBuf, BufSize, Default) GetRegistryStringW(HKEY_CURRENT_USER, L"Software\\Team17SoftwareLTD\\WormsArmageddon\\Options\\", OptionName, OutBuf, BufSize, Default)
#define GetWARegOptionDwordA(OptionName, Default) GetRegistryDwordA(HKEY_CURRENT_USER, "Software\\Team17SoftwareLTD\\WormsArmageddon\\Options\\", OptionName, Default)
#define GetWARegOptionDwordW(OptionName, Default) GetRegistryDwordW(HKEY_CURRENT_USER, L"Software\\Team17SoftwareLTD\\WormsArmageddon\\Options\\", OptionName, Default)
#define GetWARegPathA(OutBuf, BufSize, Default) GetRegistryStringA(HKEY_CURRENT_USER, "Software\\Team17SoftwareLTD\\WormsArmageddon\\", "PATH", OutBuf, BufSize, Default)
#define GetWARegPathW(OutBuf, BufSize, Default) GetRegistryStringW(HKEY_CURRENT_USER, L"Software\\Team17SoftwareLTD\\WormsArmageddon\\", "PATH", OutBuf, BufSize, Default)

#define WriteWARegOptionDwordA(OptionName, NewValue) WriteRegistryDwordA(HKEY_CURRENT_USER, "Software\\Team17SoftwareLTD\\WormsArmageddon\\Options\\", OptionName, NewValue)
#define WriteWARegOptionDwordW(OptionName, NewValue) WriteRegistryDwordW(HKEY_CURRENT_USER, L"Software\\Team17SoftwareLTD\\WormsArmageddon\\Options\\", OptionName, NewValue)

#ifdef UNICODE
#define GetPathUnderModule  GetPathUnderModuleW
#define GetRegistryString GetRegistryStringW
#define GetRegistryDword GetRegistryDwordW
#define GetWARegOptionString GetWARegOptionStringW
#define GetWARegOptionDword GetWARegOptionDwordW
#define GetWARegPath GetWARegPathW
#define WritePrivateProfileInt  WritePrivateProfileIntW
#define WriteRegistryDword WriteRegistryDwordW
#define WriteWARegOptionDword WriteWARegOptionDwordW
#define GenericBox GenericBoxW
#define InfoBox InfoBoxW
#define ErrorBox ErrorBoxW
#define WarningBox WarningBoxW
#define PatchMemString PatchMemStringW
#define DispChangeErrorStr DispChangeErrorStrW

#else
#define GetPathUnderModule  GetPathUnderModuleA
#define GetRegistryString GetRegistryStringA
#define GetRegistryDword GetRegistryDwordA
#define GetWARegOptionString GetWARegOptionStringA
#define GetWARegOptionDword GetWARegOptionDwordA
#define GetWARegPath GetWARegPathA
#define WritePrivateProfileInt  WritePrivateProfileIntA
#define WriteRegistryDword WriteRegistryDwordA
#define WriteWARegOptionDword WriteWARegOptionDwordA
#define GenericBox GenericBoxA
#define InfoBox InfoBoxA
#define ErrorBox ErrorBoxA
#define WarningBox WarningBoxA
#define PatchMemString PatchMemStringA
#define DispChangeErrorStr DispChangeErrorStrA
#endif

QWORD GetFileVersionQ();
BOOL DWMEnabled();
BOOL EnableDPIAwareness();
BOOL WritePrivateProfileIntA(LPCSTR, LPCSTR, int, LPCSTR);
BOOL WritePrivateProfileIntW(LPCWSTR, LPCWSTR, int, LPCWSTR);
LPSTR GetPathUnderModuleA(HMODULE, LPSTR, LPCSTR);
LPWSTR GetPathUnderModuleW(HMODULE, LPWSTR, LPCWSTR);
LSTATUS GetRegistryStringA(HKEY, LPCSTR, LPCSTR, LPSTR, DWORD, LPCSTR);
LSTATUS GetRegistryStringW(HKEY, LPCSTR, LPCSTR, LPSTR, DWORD, LPCSTR);
DWORD GetRegistryDwordA(HKEY, LPCSTR, LPCSTR, DWORD);
DWORD GetRegistryDwordW(HKEY, LPCWSTR, LPCWSTR, DWORD);
LSTATUS WriteRegistryDwordA(HKEY, LPCSTR, LPCSTR, DWORD);
LSTATUS WriteRegistryDwordW(HKEY, LPCWSTR, LPCWSTR, DWORD);

LPSTR GetPathUnderExeA(CHAR[MAX_PATH], LPCSTR);
LPWSTR GetPathUnderExeW(WCHAR[MAX_PATH], LPCWSTR);

BOOL PatchMemData(ULONG, PVOID, ULONG);
BOOL PatchMemQword(ULONG, QWORD);
BOOL PatchMemDword(ULONG, DWORD);
BOOL PatchMemWord(ULONG, WORD);
BOOL PatchMemByte(ULONG, BYTE);
BOOL PatchMemFloat(ULONG, FLOAT);
BOOL PatchMemDouble(ULONG, DOUBLE);
BOOL PatchMemStringA(ULONG, LPSTR);
BOOL PatchMemStringW(ULONG, LPWSTR);

#define SteamCheck WASteamCheck
bool WASteamCheck();

bool DataPatternCompare(LPCBYTE, LPCBYTE, LPCSTR);
ULONG FindPattern(PBYTE, LPSTR, ULONG, ULONG);
ULONG FindPatternPrecise(PBYTE, ULONG, ULONG, ULONG);

bool LockCurrentInstance(LPCTSTR);
bool LockGlobalInstance(LPCTSTR);

int GetCursorCounter();
void SetCursorCounter(int value);

int Mprintf(UINT uType, LPCSTR lpCaption, LPCSTR Format, ...);
LONG SetScreenRes(int cx, int cy);
BOOL SetWindowTransparencyLevel(HWND hWnd, BYTE bLevel);
BOOL IsNullRect(LPRECT lpRect);
BOOL RectCat(LPRECT Dest, LPRECT Source);

LPCSTR DispChangeErrorStrA(LONG ErrorCode);
LPCWSTR DispChangeErrorStrW(LONG ErrorCode);

int LogToFileA(FILE* pFile, LPCSTR Format, ...);
BOOL CmdOption(LPCSTR lpCmdOption);

typedef class PEInfo
{
public:
	PEInfo(HMODULE hModule);
	~PEInfo();

	BOOL PtrInCode(PVOID ptr);
	BOOL PtrInData(PVOID ptr);

	IMAGE_DOS_HEADER* DOS;
	IMAGE_NT_HEADERS* NT;
	IMAGE_FILE_HEADER* FH;
	IMAGE_OPTIONAL_HEADER* OPT;
} *PPEInfo;
