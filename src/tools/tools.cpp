//DLL tools for W:A by StepS.

#include "tools.h"

QWORD GetModuleVersion(HMODULE hModule)
{
	char WApath[MAX_PATH]; DWORD h;
	WORD V1,V2,V3,V4;
	GetModuleFileNameA(hModule,WApath,MAX_PATH);
	DWORD Size = GetFileVersionInfoSizeA(WApath,&h);
	if(Size)
	{
		void* Buf = malloc(Size);
		GetFileVersionInfoA(WApath,h,Size,Buf);
		VS_FIXEDFILEINFO *Info; DWORD Is;
		if(VerQueryValueA(Buf, "\\", (LPVOID*)&Info, (PUINT)&Is))
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
		free(Buf);
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

DOUBLE GetPrivateProfileDoubleA(LPCSTR lpAppName, LPCSTR lpKeyName, DOUBLE dDefault, LPCSTR lpFileName)
{
	CHAR lpString[32], lpDefault[32];
	sprintf_s(lpDefault, "%f", dDefault);
	GetPrivateProfileStringA(lpAppName, lpKeyName, lpDefault, lpString, 32, lpFileName);
	return atof(lpString);
}

DOUBLE GetPrivateProfileDoubleW(LPCWSTR lpAppName, LPCWSTR lpKeyName, DOUBLE dDefault, LPCWSTR lpFileName)
{
	WCHAR lpString[32], lpDefault[32];
	swprintf_s(lpDefault, L"%f", dDefault);
	GetPrivateProfileStringW(lpAppName, lpKeyName, lpDefault, lpString, 32, lpFileName);
	return _wtof(lpString);
}

LPSTR GetPathUnderModuleA(HMODULE hModule, CHAR OutBuf[MAX_PATH], LPCSTR FileName)
{
	CHAR* dirend;
	if (GetModuleFileNameA(hModule, OutBuf, MAX_PATH))
	if ((dirend = strrchr(OutBuf, '\\')) != 0)
	{
		strcpy_s(dirend + 1, MAX_PATH, FileName);
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
		wcscpy_s(dirend + 1, MAX_PATH, FileName);
		return OutBuf;
	}
	return 0;
}

QWORD GetFileSizeQ(HANDLE hFile)
{
	DWORD low, high;
	low = GetFileSize(hFile, &high);
	return MAKELONGLONG(high, low);
}

BOOL __stdcall PatchMemData(PVOID pAddr, size_t buf_len, PVOID pNewData, size_t data_len)
{
	if (!buf_len || !data_len || !pNewData || !pAddr || buf_len < data_len)
	{
		SetLastError(ERROR_INVALID_PARAMETER);
		return 0;
	}
	DWORD dwLastProtection;
	if (!VirtualProtect((void*)pAddr, data_len, PAGE_EXECUTE_READWRITE, &dwLastProtection))
		return 0;
	memcpy_s(pAddr, buf_len, pNewData, data_len);
	return VirtualProtect((void*)pAddr, data_len, dwLastProtection, &dwLastProtection);
}

BOOL __stdcall PatchMemStringA(PVOID pAddr, size_t dest_len, LPSTR lpString) { return PatchMemData(pAddr, dest_len, lpString, strlen(lpString) + 1); }
BOOL __stdcall PatchMemStringW(PVOID pAddr, size_t dest_len, LPWSTR lpString) { return PatchMemData(pAddr, dest_len * 2, lpString, wcslen(lpString) * 2 + 2); }

BOOL __stdcall InsertJump(PVOID pDest, size_t dwPatchSize, PVOID pCallee, DWORD dwJumpType)
{
	if (dwPatchSize >= 5 && pDest)
	{
		DWORD OpSize = 5, OpCode = 0xE9;
		PBYTE dest = (PBYTE)pDest;
		switch (dwJumpType)
		{
		case IJ_PUSHRET:
			OpSize = 6;
			OpCode = 0x68;
			break;
		case IJ_FARJUMP:
			OpSize = 7;
			OpCode = 0xEA;
			break;
		case IJ_FARCALL:
			OpSize = 7;
			OpCode = 0x9A;
			break;
		case IJ_CALL:
			OpSize = 5;
			OpCode = 0xE8;
			break;
		default:
			OpSize = 5;
			OpCode = 0xE9;
			break;
		}

		if (dwPatchSize < OpSize)
			return 0;

		PatchMemVal(dest, (BYTE)OpCode);

		switch (OpSize)
		{
		case 7:
			PatchMemVal(dest + 1, pCallee);
			WORD w_cseg;
			__asm mov [w_cseg], cs;
			PatchMemVal(dest + 5, w_cseg);
			break;
		case 6:
			PatchMemVal(dest + 1, pCallee);
			PatchMemVal<BYTE>(dest + 5, 0xC3);
			break;
		default:
			PatchMemVal(dest + 1, (ULONG_PTR)pCallee - (ULONG_PTR)pDest - 5);
			break;
		}

		for (size_t i = OpSize; i < dwPatchSize; i++)
			PatchMemVal<BYTE>(dest + i, 0x90);
	}
	return 0;
}

bool DataPatternCompare(LPCBYTE pData, LPCBYTE bMask, LPCSTR szMask)
{
	for (; *szMask; ++szMask, ++pData, ++bMask)
	if (*szMask == 'x' && *pData != *bMask)
		return false;
	return (*szMask) == 0;
}

ULONG_PTR FindPattern(PBYTE bMask, LPSTR szMask, ULONG_PTR dwAddress, size_t dwLen, BOOL backwards)
{
	if (!backwards)
	{
		for (size_t i = 0; i < dwLen - strlen(szMask); i++)
			if (DataPatternCompare((PBYTE)(dwAddress + i), bMask, szMask))
				return (ULONG)(dwAddress + i);
	}
	else
	{
		for (size_t i = dwLen - strlen(szMask); i > 0; i--)
			if (DataPatternCompare((PBYTE)(dwAddress - i), bMask, szMask))
				return (ULONG)(dwAddress - i);
	}
	return 0;
}

ULONG_PTR FindPatternPrecise(PBYTE bMask, size_t dwCount, ULONG_PTR dwAddress, size_t dwLen, BOOL backwards)
{
	if (!backwards)
	{
		for (size_t i = 0; i < dwLen - dwCount; i++)
			if (!memcmp((LPCVOID)(dwAddress + i), bMask, dwCount))
				return (ULONG)(dwAddress + i);
	}
	else
	{
		for (size_t i = dwLen - dwCount; i > 0; i--)
			if (!memcmp((LPCVOID)(dwAddress - i), bMask, dwCount))
				return (ULONG)(dwAddress - i);
	}
	return 0;
}

PEInfo::PEInfo(HMODULE hModule)
{
	Reset(hModule);
}

void PEInfo::Reset(HMODULE hModule)
{
	hModule = hModule == 0 ? GetModuleHandleA(0) : hModule;
	Handle = hModule;
	DOS = (IMAGE_DOS_HEADER*)hModule;
	NT = (IMAGE_NT_HEADERS*)((ULONG_PTR)DOS + DOS->e_lfanew);
	FH = (IMAGE_FILE_HEADER*)&NT->FileHeader;
	OPT = (IMAGE_OPTIONAL_HEADER*)&NT->OptionalHeader;
}

ULONG_PTR PEInfo::Offset(size_t off)
{
	return (ULONG_PTR)Handle + off;
}

BOOL PEInfo::PtrInCode(PVOID ptr)
{
	return
		ULONG_PTR(ptr) >= Offset(OPT->BaseOfCode) &&
		ULONG_PTR(ptr) <  Offset(OPT->BaseOfCode) + OPT->SizeOfCode;
}

BOOL PEInfo::PtrInData(PVOID ptr)
{
	return
		ULONG_PTR(ptr) >= Offset(OPT->BaseOfData) &&
		ULONG_PTR(ptr) <  Offset(OPT->BaseOfData) + OPT->SizeOfInitializedData + OPT->SizeOfUninitializedData;
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
	ULONG_PTR saddr;
	QWORD WAVersion = GetWAVersion();
	if (WAVersion < QV(3,6,31,2))
		return 0;

	switch (WAVersion)
	{
		case QV(3,7,2,1):
			saddr = 0x006263BE;
			goto DefinedCheck;
			break;

		default:
		{
			PEInfo EXE(0);
			ULONG_PTR VerStrSearchResult = FindPatternPrecise((PBYTE)"VERSION Worms Armageddon ",
				25, EXE.Offset(EXE.OPT->BaseOfData), EXE.OPT->SizeOfInitializedData);
			EXE.~EXE();
			if (VerStrSearchResult != NULL)
			if (strstr((char*)VerStrSearchResult, "Steam"))
				return 1;
			break;
		}
	}

	return 0;

DefinedCheck:
	return *(PDWORD)saddr == 'maet' || *(PDWORD)saddr == 0;
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
	if (result >= 0)
		result = MessageBoxA(0, buf, lpCaption, uType);
	// If a vsprintf error occurs, the vsprintf error code (<0) is returned. Otherwise, one of the MessageBoxA codes (>=0)
	return result;
}

BOOL FileExistsA(LPCSTR lpFileName)
{
	if (!lpFileName)
		return 0;
	DWORD fattr = GetFileAttributesA(lpFileName);
	if (fattr != INVALID_FILE_ATTRIBUTES && !(fattr & FILE_ATTRIBUTE_DIRECTORY))
		return 1;
	return 0;
}

BOOL FileExistsW(LPCWSTR lpFileName)
{
	if (!lpFileName)
		return 0;
	DWORD fattr = GetFileAttributesW(lpFileName);
	if (fattr != INVALID_FILE_ATTRIBUTES && !(fattr & FILE_ATTRIBUTE_DIRECTORY))
		return 1;
	return 0;
}

BOOL DirectoryExistsA(LPCSTR lpDirName)
{
	if (!lpDirName)
		return 0;
	DWORD fattr = GetFileAttributesA(lpDirName);
	if (fattr != INVALID_FILE_ATTRIBUTES && (fattr & FILE_ATTRIBUTE_DIRECTORY))
		return 1;
	return 0;
}

BOOL DirectoryExistsW(LPCWSTR lpDirName)
{
	if (!lpDirName)
		return 0;
	DWORD fattr = GetFileAttributesW(lpDirName);
	if (fattr != INVALID_FILE_ATTRIBUTES && (fattr & FILE_ATTRIBUTE_DIRECTORY))
		return 1;
	return 0;
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

BOOL UnadjustWindowRectEx(LPRECT lpRect, DWORD dwStyle, BOOL fMenu, DWORD dwExStyle)
{
	RECT rc;
	SetRectEmpty(&rc);
	BOOL fRc = AdjustWindowRectEx(&rc, dwStyle, fMenu, dwExStyle);
	if (fRc)
	{
		lpRect->left -= rc.left;
		lpRect->top -= rc.top;
		lpRect->right -= rc.right;
		lpRect->bottom -= rc.bottom;
	}
	return fRc;
}

BOOL UnadjustWindowRect(LPRECT lpRect, DWORD dwStyle, BOOL fMenu)
{
	return UnadjustWindowRectEx(lpRect, dwStyle, fMenu, 0);
}

void PosToRect(RECT &Rect, int x, int y, int cx, int cy)
{
	Rect = { x, y, x + cx, y + cy };
}

void RectToPos(RECT &Rect, int &x, int &y, int &cx, int &cy)
{
	x = Rect.left;
	y = Rect.top;
	cx = Rect.right - Rect.left;
	cy = Rect.bottom - Rect.top;
}

BOOL AdjustPosViaRectEx(int& x, int&y, int& cx, int& cy, DWORD dwStyle, BOOL fMenu, DWORD dwExStyle)
{
	RECT rc;
	PosToRect(rc, x, y, cx, cy);
	BOOL result = AdjustWindowRectEx(&rc, dwStyle, fMenu, dwExStyle);
	RectToPos(rc, x, y, cx, cy);
	return result;
}

BOOL AdjustPosViaRect(int& x, int&y, int& cx, int& cy, DWORD dwStyle, BOOL fMenu)
{
	return AdjustPosViaRectEx(x, y, cx, cy, dwStyle, fMenu, 0);
}

BOOL UnadjustPosViaRectEx(int& x, int&y, int& cx, int& cy, DWORD dwStyle, BOOL fMenu, DWORD dwExStyle)
{
	RECT rc;
	PosToRect(rc, x, y, cx, cy);
	BOOL result = UnadjustWindowRectEx(&rc, dwStyle, fMenu, dwExStyle);
	RectToPos(rc, x, y, cx, cy);
	return result;
}

BOOL UnadjustPosViaRect(int& x, int&y, int& cx, int& cy, DWORD dwStyle, BOOL fMenu)
{
	return UnadjustPosViaRectEx(x, y, cx, cy, dwStyle, fMenu, 0);
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

char *_strtrim(char *lpStr)
{
	while (isspace(*lpStr))
		lpStr++;

	if (*lpStr == 0)
		return lpStr;

	char *strEnd = lpStr + strlen(lpStr) - 1;
	while (strEnd > lpStr && isspace(*strEnd)) strEnd--;

	*(strEnd + 1) = 0;

	return lpStr;
}
