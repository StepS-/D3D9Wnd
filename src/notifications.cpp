
#include "notifications.h"
#include "tools/tools.h"
char walanguage[32];

void M_TooSmallWidth(HWND hWnd)
{
	ClipCursor(NULL);
	ShowWindow(hWnd, SW_MINIMIZE);
	if (GameLanguage("Russian")) MessageBox(0, "���� �� ����� ��������, ��������� ������ ������ � �������� ���������� ���� 143. ����������, ��������� � �� ���������� ��������.", "������ ������ D3D9Wnd", MB_OK | MB_ICONERROR);
	else if (GameLanguage("Italian")) MessageBox(0, "Il gioco non pu� avviarsi perch� la larghezza dello schermo settata � minore di 143. � necessario modificarla con un valore pi� appropriato.", "Errore D3D9Wnd", MB_OK | MB_ICONERROR);
	else MessageBox(0, "The game cannot start because the specified screen width is lower than 143. Please change it to an appropriate value.", "D3D9Wnd error", MB_OK | MB_ICONERROR);
}

void M_WndmodeDetected()
{
	if (GameLanguage("Russian")) MessageBox(0, "� ����� ����� � W:A ������������ ����� ��� ������ �������� ������ (D3D9 � wkWndMode). ������� ���� �� ���. ������ ������ ��������� ���� ������.", "������", MB_OK | MB_ICONERROR);
	else if (GameLanguage("Italian")) MessageBox(0, "D3D9 e wkWndMode sono entrambi presenti nella tua cartella. � necessario eliminarne uno.", "Errore Critico.", MB_OK | MB_ICONERROR);
	else ErrorBox("You have both D3D9 and wkWndMode in your folder. Delete one please.", "Critical failure.");
}

void M_D3D9Disabled()
{
	if (GameLanguage("Russian")) MessageBox(0, "����������, �������� ���� �� ��������� Direct3D 9 ����� ���� �������������� �������� ����, ���� ����������� ��������������� ������ ������� �� ����� \"Tweaks\". ������ ������ ��������� ���� ������.", "������ ������ D3D9Wnd", MB_OK | MB_ICONERROR);
	else if (GameLanguage("Italian")) MessageBox(0, "Per favore abilita uno dei renderer Direct3D 9 in Advanced Settings oppure utilizzando i corrispettivi files presenti nella cartella \"Tweaks\".", "Errore D3D9Wnd", MB_OK | MB_ICONERROR);
	else ErrorBox("Please enable one of the Direct3D 9 renderers either in the Advanced Settings or by using the corresponding files from \"Tweaks\" folder. I will exit now.", "D3D9Wnd error");
}

void M_TooOld()
{
	if (RegistryLanguage("Russian")) MessageBox(0, "� ���������, ���� ������ ���� ������� ������ ��� ����, ����� ������������ D3D9Wnd. ����������, �������� ���� �� ��������� ������.", "� ��� ��������, �������!", MB_OK | MB_ICONERROR);
	else ErrorBox("Unfortunately, your version of the game is too old for using D3D9Wnd. Please update your game to the latest version.", "No, just no.");
}

void M_HardwareCursorsDisabled()
{
	if (GameLanguage("Russian")) MessageBox(0, "���������� ������ ���� ��� ��������, ����� ��������� ����� �������� ���������.", "���������� ������ D3D9Wnd", MB_OK | MB_ICONINFORMATION);
	else if (GameLanguage("Italian")) MessageBox(0, "Ho disabilitato il cursore per permettere il funzionamento della modalit� estesa.", "Informazione D3D9Wnd", MB_OK | MB_ICONINFORMATION);
	else InfoBox("I have disabled the Hardware Cursors to allow some things to work.", "D3D9Wnd information");
}

void M_RecommendHardwareCursorsDisable()
{
	if (RegistryLanguage("Russian")) MessageBox(0, "��� ���������� ������ �����������/���������������/����������������� ���� ������� ��������� ����� \"���������� ������ ����\" � �������������� ���������� ����.", "���������� ������ D3D9Wnd", MB_OK | MB_ICONWARNING);
	else WarningBox("In order for the stretched/centered/custom frontend to work properly, please disable \"Hardware Cursors\" in the Advanced Options screen.", "D3D9Wnd information");
}

void M_SetMultiMonResLight(int nWidth, int nHeight)
{
	if (RegistryLanguage("Russian")) Mprintfi("���������� ������ D3D9Wnd", "������, � �������, ��� ����������� ������ ����������������� ����������� ����. � ����� ���������� ����� ���� �� %ux%u. ����������, ������������� ����, ����� ����������� ���������.", nWidth, nHeight);
	else Mprintfi("D3D9Wnd information", "Hello, and thanks for using the Multi-Monitor feature of D3D9Wnd. I have set your game resolution to %ux%u. Please restart the game to confirm the changes.", nWidth, nHeight);
}

void M_LowBPP(UINT dwDepth)
{
	if (RegistryLanguage("Russian")) Mprintfe("������ ������ D3D9Wnd", "��������, �� ������� ����� � ����� ������� (%u ����� �� �������) ������� ������ ��� ����, ����� ������������ D3D9Wnd. ����������, ������� ������� ����� ���� �� �� 32 ���� �� �������.", dwDepth);
	else Mprintfe("D3D9Wnd error", "Sorry, but the colour depth of your system (%u bits per pixel) is too low for using D3D9Wnd. Please set colour depth to at least 32 bits per pixel.", dwDepth);
}

void M_UnsupportedFullscreen(int nWidth, int nHeight, LONG dwErrorCode)
{
	if (RegistryLanguage("Russian"))
		Mprintfw("�������������� ������ D3D9Wnd",
		"��������: �� ������� ��������� ���� � ������������� ����� � ����������� %ux%u.\n"
		"������: %s\n"
		"��������, ��� ���������� �� �������������� ����� ��������, ��������� ��� �����������/����������. ������ ��������� ������������ � ������� �����.",
		nWidth, nHeight, DispChangeErrorStrA(dwErrorCode));
	else Mprintfw("D3D9Wnd warning",
		"Warning: failed to set the required screen resolution of %ux%u for frontend.\n"
		"Error: %s\n"
		"It's probably unsupported by your system, monitor or GPU/drivers. Switching to windowed mode.",
		nWidth, nHeight, DispChangeErrorStrA(dwErrorCode));
}
