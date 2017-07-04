#pragma once

#include <Windows.h>
#include <d3d9.h>
#include <dsound.h>
#include <ddraw.h>

typedef IDirect3D9*(WINAPI *D3D9CREATE)(UINT);
typedef HRESULT(WINAPI *D3D9_CREATEDEVICE)(IDirect3D9*, UINT, D3DDEVTYPE, HWND, DWORD, D3DPRESENT_PARAMETERS*, IDirect3DDevice9**);
typedef HRESULT(WINAPI *D3D9DEVICE_PRESENT)(IDirect3DDevice9*, const RECT*, const RECT*, HWND, const RGNDATA*);
typedef HRESULT(WINAPI *D3D9DEVICE_TESTCOOPERATIVELEVEL)(IDirect3DDevice9*);
typedef HRESULT(WINAPI *D3D9DEVICE_RESET)(IDirect3DDevice9*, D3DPRESENT_PARAMETERS*);
typedef HRESULT(WINAPI *D3D9DEVICE_GETRASTERSTATUS)(IDirect3DDevice9*, UINT, D3DRASTER_STATUS*);
typedef ULONG(WINAPI *IUNKNOWN_RELEASE)(IUnknown*);
typedef HRESULT(WINAPI *DSOUNDCREATE)(LPCGUID, LPDIRECTSOUND*, LPUNKNOWN);
typedef HRESULT(WINAPI *DSOUND_CREATESOUNDBUFFER)(IDirectSound*, LPCDSBUFFERDESC, LPDIRECTSOUNDBUFFER*, LPUNKNOWN);

extern HRESULT(WINAPI *DirectDrawCreateNext)(GUID*, LPDIRECTDRAW*, IUnknown*);
extern HRESULT(WINAPI *DirectDrawCreateExNext)(GUID*, LPDIRECTDRAW*, REFIID, IUnknown*);

extern D3D9CREATE Direct3DCreate9Next;
extern D3D9_CREATEDEVICE D3D9CreateDeviceNext;
extern D3D9DEVICE_PRESENT D3D9PresentNext;
extern D3D9DEVICE_TESTCOOPERATIVELEVEL D3D9TestCooperativeLevelNext;
extern D3D9DEVICE_RESET D3D9ResetNext;
extern D3D9DEVICE_GETRASTERSTATUS D3D9GetRasterStatusNext;
extern IUNKNOWN_RELEASE D3D9ReleaseNext;
extern DSOUNDCREATE DSoundCreateNext;
extern DSOUND_CREATESOUNDBUFFER DSoundCreateSoundBufferNext;

HRESULT WINAPI DirectDrawCreateHook(GUID* lpGUID, LPDIRECTDRAW* lplpDD, IUnknown* pUnkOuter);
HRESULT WINAPI DirectDrawCreateExHook(GUID* lpGuid, LPDIRECTDRAW* lplpDD, REFIID iid, IUnknown* pUnkOuter);
HRESULT WINAPI D3D9ResetHook(IDirect3DDevice9* pthis, D3DPRESENT_PARAMETERS *pParams);
ULONG WINAPI D3D9ReleaseHook(IUnknown* pthis);
HRESULT WINAPI D3D9GetRasterStatusHook(IDirect3DDevice9 *pthis, UINT iSwapChain, D3DRASTER_STATUS *pRasterStatus);
IDirect3D9* WINAPI Direct3DCreate9Hook(UINT SDKVersion);
HRESULT WINAPI D3D9CreateDeviceHook(IDirect3D9 *pthis, UINT Adapter, D3DDEVTYPE DeviceType, HWND hFocusWindow,
	DWORD BehaviorFlags, D3DPRESENT_PARAMETERS *pParams, IDirect3DDevice9 **ppReturnedDeviceInterface);
HRESULT WINAPI DSoundCreateHook(LPCGUID lpcGuidDevice, LPDIRECTSOUND *ppDS, LPUNKNOWN pUnkOuter);
HRESULT WINAPI DSoundCreateSoundBufferHook(IDirectSound* pthis, LPDSBUFFERDESC pcDSBufferDesc,
	LPDIRECTSOUNDBUFFER * ppDSBuffer, LPUNKNOWN pUnkOuter);


BOOL __stdcall SetWndParam(HWND hWnd, HWND hWndInsertAfter, int X, int Y, int cx, int cy, UINT uFlags);
