//DLL tools for W:A.

#include "tools.h"

QWORD GetFileVersionQ()
{
	char WApath[MAX_PATH];DWORD h;
	WORD V1,V2,V3,V4;
	GetModuleFileName(0,WApath,MAX_PATH);
	DWORD Size=GetFileVersionInfoSize(WApath,&h);
	if(Size)
	{
		void* Buf=malloc(Size);
		GetFileVersionInfo(WApath,h,Size,Buf);
		VS_FIXEDFILEINFO *Info;DWORD Is;
		if(VerQueryValue(Buf,"\\",(LPVOID*)&Info,(PUINT)&Is))
		{
			if(Info->dwSignature==0xFEEF04BD)
			{
				V1=HIWORD(Info->dwFileVersionMS);
				V2=LOWORD(Info->dwFileVersionMS);
				V3=HIWORD(Info->dwFileVersionLS);
				V4=LOWORD(Info->dwFileVersionLS);
					return MAKEQWORD(V1, V2, V3, V4);
			}
		}
	}
	return 0;
}

BOOL DWMEnabled()
{
	BOOL result = 0;
#ifdef VISTAUP
	DwmIsCompositionEnabled(&result);
#else
	if (HMODULE DwmApi = LoadLibrary("dwmapi.dll"))
	{
		HRESULT(WINAPI *_DwmIsCompositionEnabled)(BOOL *pfEnabled) = (HRESULT(WINAPI *)(BOOL*))GetProcAddress(DwmApi, "DwmIsCompositionEnabled");
		if (_DwmIsCompositionEnabled)_DwmIsCompositionEnabled(&result);
		FreeLibrary(DwmApi);
	}
#endif
	return result;
}

BOOL EnableDPIAwareness()
{
	BOOL result = 0;
#ifdef VISTAUP
	if (!IsProcessDPIAware())
		result = SetProcessDPIAware();
#else
	HMODULE hUser32 = LoadLibrary("user32.dll");
	FARPROC isDPIAware = GetProcAddress(hUser32, "IsProcessDPIAware");
	if (isDPIAware)
	{
		if (!isDPIAware())
		{
			FARPROC setDPIAware = GetProcAddress(hUser32, "SetProcessDPIAware");
			if (setDPIAware) result = setDPIAware();
		}
	}
	FreeLibrary(hUser32);
#endif
	return result;
}

BOOL WritePrivateProfileIntA(LPCSTR lpAppName, LPCSTR lpKeyName, int nInteger, LPCSTR lpFileName)
{
	CHAR lpString[32];
	sprintf_s(lpString, "%d", nInteger);
	return WritePrivateProfileStringA(lpAppName, lpKeyName, lpString, lpFileName);
}

BOOL WritePrivateProfileIntW(LPCWSTR lpAppName, LPCWSTR lpKeyName, int nInteger, LPCWSTR lpFileName)
{
	WCHAR lpString[32];
	swprintf_s(lpString, L"%d", nInteger);
	return WritePrivateProfileStringW(lpAppName, lpKeyName, lpString, lpFileName);
}

LPSTR GetPathUnderModuleA(HMODULE hModule, CHAR OutBuf[MAX_PATH], LPCSTR FileName)
{
	CHAR* dirend;
	if (GetModuleFileNameA(hModule, OutBuf, MAX_PATH))
	if ((dirend = strrchr(OutBuf, '\\')) != 0)
	{
		strcpy_s(dirend + sizeof(CHAR), MAX_PATH, FileName);
		return OutBuf;
	}
	return 0;
}

LPWSTR GetPathUnderModuleW(HMODULE hModule, WCHAR OutBuf[MAX_PATH], LPCWSTR FileName)
{
	WCHAR* dirend;
	if (GetModuleFileNameW(hModule, OutBuf, MAX_PATH))
	if ((dirend = wcsrchr(OutBuf, L'\\')) != 0)
	{
		wcscpy_s(dirend + sizeof(WCHAR), MAX_PATH, FileName);
		return OutBuf;
	}
	return 0;
}

BOOL PatchMemData(ULONG dwAddr, PVOID pNewData, ULONG dwLen)
{
	if (!dwLen || !pNewData || !dwAddr)
	{
		SetLastError(ERROR_INVALID_PARAMETER);
		return 0;
	}
	DWORD dwLastProtection;
	if (!VirtualProtect((void*)dwAddr, dwLen, PAGE_READWRITE, &dwLastProtection))
		return 0;
	switch (dwLen)
	{
		case 1:
			*(PBYTE)dwAddr = *(PBYTE)pNewData;
			break;
		case 2:
			*(PWORD)dwAddr = *(PWORD)pNewData;
			break;
		case 4:
			*(PDWORD)dwAddr = *(PDWORD)pNewData;
			break;
		case 8:
			*(PQWORD)dwAddr = *(PQWORD)pNewData;
			break;
		default:
			memcpy_s((PVOID)dwAddr, dwLen, pNewData, dwLen);
	}
	
	return VirtualProtect((void*)dwAddr, dwLen, dwLastProtection, &dwLastProtection);
}

BOOL PatchMemQword(ULONG dwAddr, QWORD qNewValue) { return PatchMemData(dwAddr, &qNewValue, sizeof(QWORD)); }
BOOL PatchMemDword(ULONG dwAddr, DWORD dwNewValue) { return PatchMemData(dwAddr, &dwNewValue, sizeof(DWORD)); }
BOOL PatchMemWord(ULONG dwAddr, WORD wNewValue) { return PatchMemData(dwAddr, &wNewValue, sizeof(WORD)); }
BOOL PatchMemByte(ULONG dwAddr, BYTE bNewValue) { return PatchMemData(dwAddr, &bNewValue, sizeof(BYTE)); }
BOOL PatchMemStringA(ULONG dwAddr, LPSTR lpString) { return PatchMemData(dwAddr, lpString, strlen(lpString) + sizeof(CHAR)); }
BOOL PatchMemStringW(ULONG dwAddr, LPWSTR lpString) { return PatchMemData(dwAddr, lpString, wcslen(lpString) + sizeof(WCHAR)); }

BOOL PatchMemFloat(ULONG dwAddr, FLOAT fNewValue)
{ 
	DWORD dwLastProtection;
	if (!VirtualProtect((void*)dwAddr, sizeof(FLOAT), PAGE_READWRITE, &dwLastProtection))
		return 0;
	*(FLOAT*)dwAddr = fNewValue;
	return VirtualProtect((void*)dwAddr, sizeof(FLOAT), dwLastProtection, &dwLastProtection);
}
BOOL PatchMemDouble(ULONG dwAddr, DOUBLE dNewValue)
{
	DWORD dwLastProtection;
	if (!VirtualProtect((void*)dwAddr, sizeof(DOUBLE), PAGE_READWRITE, &dwLastProtection))
		return 0;
	*(DOUBLE*)dwAddr = dNewValue;
	return VirtualProtect((void*)dwAddr, sizeof(DOUBLE), dwLastProtection, &dwLastProtection);
}

bool DataPatternCompare(LPCBYTE pData, LPCBYTE bMask, LPCSTR szMask)
{
	for (; *szMask; ++szMask, ++pData, ++bMask)
	if (*szMask == 'x' && *pData != *bMask)
		return false;
	return (*szMask) == 0;
}

ULONG FindPattern(PBYTE bMask, LPSTR szMask, ULONG dwAddress, ULONG dwLen)
{
	for (ULONG i = 0; i < dwLen - strlen(szMask); i++)
	if (DataPatternCompare((PBYTE)(dwAddress + i), bMask, szMask))
		return (ULONG)(dwAddress + i);
	return 0;
}

ULONG FindPatternPrecise(PBYTE bMask, ULONG dwCount, ULONG dwAddress, ULONG dwLen)
{
	for (ULONG i = 0; i < dwLen - dwCount; i++)
	if (!memcmp((LPCVOID)(dwAddress + i), bMask, dwCount))
		return (ULONG)(dwAddress + i);
	return 0;
}

PEInfo::PEInfo(HMODULE hModule)
{
	hModule = hModule == 0 ? GetModuleHandle(0) : hModule;
	DOS = (IMAGE_DOS_HEADER*)hModule;
	NT = (IMAGE_NT_HEADERS*)((DWORD)DOS + DOS->e_lfanew);
	FH = (IMAGE_FILE_HEADER*)&NT->FileHeader;
	OPT = (IMAGE_OPTIONAL_HEADER*)&NT->OptionalHeader;
}

PEInfo::~PEInfo(){}

BOOL PEInfo::PtrInCode(PVOID ptr)
{
	if (DWORD(ptr) >= OPT->ImageBase + OPT->BaseOfCode
		&& DWORD(ptr) < OPT->ImageBase + OPT->BaseOfCode + OPT->SizeOfCode)
		return true;
	return false;
}

BOOL PEInfo::PtrInData(PVOID ptr)
{
	if (DWORD(ptr) >= OPT->ImageBase + OPT->BaseOfData
		&& DWORD(ptr) < OPT->ImageBase + OPT->BaseOfData + OPT->SizeOfInitializedData + OPT->SizeOfUninitializedData)
		return true;
	return false;
}

char LocalMutexName[MAX_PATH];

bool LockCurrentInstance(LPCTSTR MutexName)
{
	sprintf_s(LocalMutexName, "P%u/%s", GetCurrentProcessId(), MutexName);
	return LockGlobalInstance(LocalMutexName);
}

bool LockGlobalInstance(LPCTSTR MutexName)
{
	if (!CreateMutex(NULL, 0, MutexName)) return 0;
	if (GetLastError() == ERROR_ALREADY_EXISTS) return 0;
	return 1;
}

int GetCursorCounter()
{
	//whatever
	ShowCursor(true);
	return ShowCursor(false);
}

void SetCursorCounter(int value)
{
	int cc = ShowCursor(value >= 0 ? true : false);
	int nc;

	if (value > cc)
	for (nc = cc; nc < value; nc++)
		ShowCursor(true);
	else if (value < cc)
	for (nc = cc; nc > value; nc--)
		ShowCursor(false);
}

bool WASteamCheck()
{
	DWORD saddr;
	QWORD WAVersion = GetWAVersion();
	if (WAVersion < QV(3,6,31,2)) return 0;

	switch (WAVersion)
	{
		case QV(3,7,2,1):
			saddr = 0x006263BE;
			goto DefinedCheck;
			break;

		default:
		{
			PEInfo EXE(0);
			ULONG VerStrSearchResult = FindPatternPrecise((PBYTE)"VERSION Worms Armageddon ",
				25, EXE.OPT->ImageBase + EXE.OPT->BaseOfData, EXE.OPT->SizeOfInitializedData);
			EXE.~EXE();
			if (VerStrSearchResult != NULL)
			if (strstr((char*)VerStrSearchResult, "Steam"))
				return 1;
			break;
		}
	}

	return 0;

DefinedCheck:
	if (*(DWORD*)saddr == 'maet')
		return 1;
	return 0;
}

LSTATUS GetRegistryStringA(HKEY hKey, LPCSTR lpSubKey, LPCSTR lpValueName, LPSTR OutBuf, DWORD BufSize, LPCSTR lpDefault)
{
	LSTATUS lResult = 0;
	DWORD regsz = REG_SZ;

#ifdef VISTAUP
	lResult = RegGetValueA(hKey, lpSubKey, lpValueName, RRF_RT_REG_SZ, &regsz, (LPBYTE)OutBuf, &BufSize);
#else
	HKEY hTargetKey;
	if ((lResult = RegOpenKeyExA(hKey, lpSubKey, 0, KEY_READ, &hTargetKey)) == ERROR_SUCCESS)
	{
		lResult = RegQueryValueExA(hTargetKey, lpValueName, NULL, &regsz, (LPBYTE)OutBuf, &BufSize);
		RegCloseKey(hTargetKey);
	}
#endif

	if (lResult != ERROR_SUCCESS)
		strcpy_s(OutBuf, BufSize, lpDefault);

	return ERROR_SUCCESS;
}

LSTATUS GetRegistryStringW(HKEY hKey, LPCWSTR lpSubKey, LPCWSTR lpValueName, LPWSTR OutBuf, DWORD BufSize, LPCWSTR lpDefault)
{
	LSTATUS lResult = 0;
	DWORD regsz = REG_SZ;

#ifdef VISTAUP
	lResult = RegGetValueW(hKey, lpSubKey, lpValueName, RRF_RT_REG_SZ, &regsz, (LPBYTE)OutBuf, &BufSize);
#else
	HKEY hTargetKey;
	if ((lResult = RegOpenKeyExW(hKey, lpSubKey, 0, KEY_READ, &hTargetKey)) == ERROR_SUCCESS)
	{
		lResult = RegQueryValueExW(hTargetKey, lpValueName, NULL, &regsz, (LPBYTE)OutBuf, &BufSize);
		RegCloseKey(hTargetKey);
	}
#endif

	if (lResult != ERROR_SUCCESS)
		wcscpy_s(OutBuf, BufSize, lpDefault);

	return ERROR_SUCCESS;
}

DWORD GetRegistryDwordA(HKEY hKey, LPCSTR lpSubKey, LPCSTR lpValueName, DWORD dwDefault)
{
	DWORD dwResult = dwDefault;
	DWORD dwSize = sizeof(DWORD);
	DWORD regdword = REG_DWORD;

#ifdef VISTAUP
	RegGetValueA(hKey, lpSubKey, lpValueName, RRF_RT_REG_DWORD, &regdword, (LPBYTE)&dwResult, &dwSize);
#else
	LSTATUS lResult = 0;
	HKEY hTargetKey;
	if ((lResult = RegOpenKeyExA(hKey, lpSubKey, 0, KEY_READ, &hTargetKey)) == ERROR_SUCCESS)
	{
		RegQueryValueExA(hTargetKey, lpValueName, NULL, &regdword, (LPBYTE)&dwResult, &dwSize);
		RegCloseKey(hTargetKey);
	}
#endif

	return dwResult;
}

DWORD GetRegistryDwordW(HKEY hKey, LPCWSTR lpSubKey, LPCWSTR lpValueName, DWORD dwDefault)
{
	DWORD dwResult = dwDefault;
	DWORD dwSize = sizeof(DWORD);
	DWORD regdword = REG_DWORD;

#ifdef VISTAUP
	RegGetValueW(hKey, lpSubKey, lpValueName, RRF_RT_REG_DWORD, &regdword, (LPBYTE)&dwResult, &dwSize);
#else
	LSTATUS lResult = 0;
	HKEY hTargetKey;
	if ((lResult = RegOpenKeyExW(hKey, lpSubKey, 0, KEY_READ, &hTargetKey)) == ERROR_SUCCESS)
	{
		RegQueryValueExW(hTargetKey, lpValueName, NULL, &regdword, (LPBYTE)&dwResult, &dwSize);
		RegCloseKey(hTargetKey);
	}
#endif

	return dwResult;
}

LSTATUS WriteRegistryDwordA(HKEY hKey, LPCSTR lpSubKey, LPCSTR lpValueName, DWORD dwNewValue)
{
	LSTATUS lResult = 0;

#ifdef VISTAUP
	lResult = RegSetKeyValueA(hKey, lpSubKey, lpValueName, REG_DWORD, (LPCBYTE)&dwNewValue, sizeof(DWORD));
#else
	HKEY hTargetKey;
	if ((lResult = RegOpenKeyExA(hKey, lpSubKey, 0, KEY_READ | KEY_SET_VALUE, &hTargetKey)) == ERROR_SUCCESS)
	{
		lResult = RegSetValueExA(hTargetKey, lpValueName, NULL, REG_DWORD, (LPCBYTE)&dwNewValue, sizeof(DWORD));
		RegCloseKey(hTargetKey);
	}
#endif

	return lResult;
}

LSTATUS WriteRegistryDwordW(HKEY hKey, LPCWSTR lpSubKey, LPCWSTR lpValueName, DWORD dwNewValue)
{
	LSTATUS lResult = 0;

#ifdef VISTAUP
	lResult = RegSetKeyValueW(hKey, lpSubKey, lpValueName, REG_DWORD, (LPCBYTE)&dwNewValue, sizeof(DWORD));
#else
	HKEY hTargetKey;
	if ((lResult = RegOpenKeyExW(hKey, lpSubKey, 0, KEY_READ | KEY_SET_VALUE, &hTargetKey)) == ERROR_SUCCESS)
	{
		lResult = RegSetValueExW(hTargetKey, lpValueName, NULL, REG_DWORD, (LPCBYTE)&dwNewValue, sizeof(DWORD));
		RegCloseKey(hTargetKey);
	}
#endif

	return lResult;
}

int Mprintf(UINT uType, LPCSTR lpCaption, LPCSTR Format, ...)
{
	int result = 0;
	va_list args;
	va_start(args, Format);
	char buf[2048];
	result = vsprintf_s(buf, Format, args);
	va_end(args);
	if (result)
		MessageBoxA(0, buf, lpCaption, uType);
	return result;
}

int LogToFileA(HANDLE hFile, LPCSTR Format, ...)
{
	int result = 0;
	if (hFile && hFile != INVALID_HANDLE_VALUE)
	{
		char buf[1900], out[2000];
		va_list args;
		va_start(args, Format);
		if (vsprintf_s(buf, Format, args))
		{
			SYSTEMTIME sysTime;
			GetSystemTime(&sysTime);
			DWORD dwWChk = 0;
			if (sprintf_s(out, "[%02u:%02u:%02u.%03u] %s\r\n", sysTime.wHour, sysTime.wMinute, sysTime.wSecond, sysTime.wMilliseconds, buf))
				result = WriteFile(hFile, out, lstrlenA(out), &dwWChk, NULL);
		}
		va_end(args);
	}
	return result;
}

LONG SetScreenRes(int cx, int cy)
{
	DEVMODE dm;
	dm.dmSize = sizeof(DEVMODE);

	dm.dmPelsWidth = cx;
	dm.dmPelsHeight = cy;

	dm.dmFields = DM_PELSWIDTH | DM_PELSHEIGHT;

	return ChangeDisplaySettings(&dm, CDS_FULLSCREEN);
}

BOOL SetWindowTransparencyLevel(HWND hWnd, BYTE bLevel)
{
	if (hWnd)
	{
		DWORD dwExStyle = GetWindowLong(hWnd, GWL_EXSTYLE);
		if (!!!(dwExStyle & WS_EX_LAYERED))
			SetWindowLong(hWnd, GWL_EXSTYLE, dwExStyle | WS_EX_LAYERED);
		return SetLayeredWindowAttributes(hWnd, 0, bLevel, LWA_ALPHA);
	}
	return 0;
}

BOOL IsNullRect(LPRECT lpRect)
{
	if (lpRect)
		return lpRect->left == 0 && lpRect->top == 0 && lpRect->right == 0 && lpRect->bottom == 0;
	return true;
}

BOOL RectCat(LPRECT Dest, LPRECT Source)
{
	if (Dest && Source)
	{
		Dest->top = MaxCap(Dest->top, Source->top);
		Dest->left = MaxCap(Dest->left, Source->left);
		Dest->bottom = MinCap(Dest->bottom, Source->bottom);
		Dest->left = MinCap(Dest->left, Source->left);
		return 1;
	}
	return 0;
}

LPCSTR DispChangeErrorStrA(LONG ErrorCode)
{
	switch (ErrorCode)
	{
	case DISP_CHANGE_SUCCESSFUL:
		return "The settings change was successful.";
	case DISP_CHANGE_BADDUALVIEW:
		return "The settings change was unsuccessful because the system is DualView capable.";
	case DISP_CHANGE_BADFLAGS:
		return "An invalid set of flags was passed in.";
	case DISP_CHANGE_BADMODE:
		return "The graphics mode is not supported.";
	case DISP_CHANGE_BADPARAM:
		return "An invalid parameter was passed in. This can include an invalid flag or combination of flags.";
	case DISP_CHANGE_FAILED:
		return "The display driver failed the specified graphics mode.";
	case DISP_CHANGE_NOTUPDATED:
		return "Unable to write settings to the registry.";
	case DISP_CHANGE_RESTART:
		return "The computer must be restarted for the graphics mode to work.";
	}

	return "Unknown error.";
}

LPCWSTR DispChangeErrorStrW(LONG ErrorCode)
{
	switch (ErrorCode)
	{
	case DISP_CHANGE_SUCCESSFUL:
		return L"The settings change was successful.";
	case DISP_CHANGE_BADDUALVIEW:
		return L"The settings change was unsuccessful because the system is DualView capable.";
	case DISP_CHANGE_BADFLAGS:
		return L"An invalid set of flags was passed in.";
	case DISP_CHANGE_BADMODE:
		return L"The graphics mode is not supported.";
	case DISP_CHANGE_BADPARAM:
		return L"An invalid parameter was passed in. This can include an invalid flag or combination of flags.";
	case DISP_CHANGE_FAILED:
		return L"The display driver failed the specified graphics mode.";
	case DISP_CHANGE_NOTUPDATED:
		return L"Unable to write settings to the registry.";
	case DISP_CHANGE_RESTART:
		return L"The computer must be restarted for the graphics mode to work.";
	}

	return L"Unknown error.";
}

BOOL CmdOption(LPCSTR lpCmdOption) // from WormKitDS
{
	if (lpCmdOption)
	{
		char* cmd = GetCommandLine();
		BOOLEAN in_QM = FALSE, in_TEXT = FALSE, in_SPACE = TRUE;
		int st, en;
		char tmp[16];
		int i = 0, j = 0, n = 1;
		while (n) {
			char a = cmd[i];
			if (in_QM) {
				if (a == '\"') { in_QM = FALSE; }
				else { if (cmd[i + 1] == 0) in_QM = FALSE; if (j < 15){ tmp[j] = a; j++; } }
			}
			else {
				switch (a) {
				case '\"':
					in_QM = TRUE;
					in_TEXT = TRUE;
					if (in_SPACE) { st = i; j = 0; }
					in_SPACE = FALSE;
					break;
				case ' ':
				case '\t':
				case '\n':
				case '\r':
				case '\0':
					if (in_TEXT) {
						tmp[j] = '\0'; en = i;
						if (!strcmp(tmp, lpCmdOption)) return 1;
					}
					in_TEXT = FALSE;
					in_SPACE = TRUE;
					break;
				default:
					in_TEXT = TRUE;
					if (in_SPACE) { st = i; j = 0; }
					if (j < 15){ tmp[j] = a; j++; }
					in_SPACE = FALSE;
					break;
				}
			}
			if (!cmd[i]) break;
			i++;
		}
	}
	return 0;
}
