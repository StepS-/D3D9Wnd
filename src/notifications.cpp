
#include "notifications.h"
#include "tools\tools.h"
char walanguage[32];

void M_TooSmallWidth(HWND hWnd)
{
	ClipCursor(NULL);
	ShowWindow(hWnd, SW_MINIMIZE);
	if (GameLanguage("Russian")) MessageBoxW(0, L"Игра не может начаться, поскольку ширина экрана в заданном разрешении ниже 143. Пожалуйста, поменяйте её на приемлемое значение.", L"Ошибка модуля D3D9Wnd", MB_OK | MB_ICONERROR);
	else if (GameLanguage("Italian")) MessageBoxW(0, L"Il gioco non può avviarsi perché la larghezza dello schermo settata è minore di 143. è necessario modificarla con un valore più appropriato.", L"Errore D3D9Wnd", MB_OK | MB_ICONERROR);
	else MessageBoxA(0, "The game cannot start because the specified screen width is lower than 143. Please change it to an appropriate value.", "D3D9Wnd error", MB_OK | MB_ICONERROR);
}

void M_WndmodeDetected()
{
	if (GameLanguage("Russian")) MessageBoxW(0, L"В вашей папке с W:A присутствуют сразу два модуля оконного режима (D3D9 и wkWndMode). Уберите один из них. Сейчас модуль прекратит свою работу.", L"Ошибка", MB_OK | MB_ICONERROR);
	else if (GameLanguage("Italian")) MessageBoxW(0, L"D3D9 e wkWndMode sono entrambi presenti nella tua cartella. è necessario eliminarne uno.", L"Errore Critico.", MB_OK | MB_ICONERROR);
	else ErrorBoxA("You have both D3D9 and wkWndMode in your folder. Delete one please.", "Critical failure.");
}

void M_D3D9Disabled()
{
	if (GameLanguage("Russian")) MessageBoxW(0, L"Пожалуйста, включите один из вариантов Direct3D 9 через меню Дополнительных настроек игры, либо активировав соответствующий скрипт реестра из папки \"Tweaks\". Сейчас модуль прекратит свою работу.", L"Ошибка модуля D3D9Wnd", MB_OK | MB_ICONERROR);
	else if (GameLanguage("Italian")) MessageBoxW(0, L"Per favore abilita uno dei renderer Direct3D 9 in Advanced Settings oppure utilizzando i corrispettivi files presenti nella cartella \"Tweaks\".", L"Errore D3D9Wnd", MB_OK | MB_ICONERROR);
	else ErrorBoxA("Please enable one of the Direct3D 9 renderers either in Advanced Settings or by using the corresponding files from the \"Tweaks\" folder. I will exit now.", "D3D9Wnd error");
}

void M_TooOld()
{
	if (GameLanguage("Russian")) MessageBoxW(0, L"К сожалению, ваша версия игры слишком старая для того, чтобы использовать D3D9Wnd. Пожалуйста, обновите игру до последней версии.", L"У нас проблема, капитан!", MB_OK | MB_ICONERROR);
	else ErrorBoxA("Unfortunately, your version of the game is too old for using D3D9Wnd. Please update your game to the latest version.", "No, just no.");
}

void M_HardwareCursorsDisabled()
{
	if (GameLanguage("Russian"))
		InfoBoxW(
			L"Аппаратный курсор мыши был отключён, чтобы Растянутое/Центрированное меню работало корректно. "
			"Чтобы вернуть аппаратный курсор мыши назад, выключите Растянутое/Центрированное меню в файле "
			"настроек модуля и снова включите аппаратный курсор мыши. Эти опции более не рекомендуются. "
			"Рекомендуем попробовать SuperFrontendHD взамен этих опций.",
			L"Оповещение модуля D3D9Wnd");
//	else if (GameLanguage("Italian")) MessageBox(0, "Ho disabilitato il cursore per permettere il funzionamento della modalità estesa.", "Informazione D3D9Wnd", MB_OK | MB_ICONINFORMATION);
	else
		InfoBoxA(
			"I have disabled the Hardware Cursors to allow Stretched/Centered Frontend to work. In order to get "
			"Hardware Cursors back, please disable Stretched/Centered Frontend in the ini file. These options "
			"are now deprecated, please take a look at SuperFrontendHD instead of these options.",
			"D3D9Wnd information");
}

void M_RecommendHardwareCursorsDisable()
{
	if (GameLanguage("Russian")) MessageBoxW(0, L"Для правильной работы растянутого/центрированного/пользовательского меню следует отключить опцию \"Аппаратный курсор мыши\" в дополнительных настройках игры.", L"Оповещение модуля D3D9Wnd", MB_OK | MB_ICONWARNING);
	else WarningBoxA("In order for the stretched/centered/custom frontend to work properly, please disable \"Hardware Cursors\" in the Advanced Options screen.", "D3D9Wnd information");
}

void M_SetMultiMonResLight(int nWidth, int nHeight)
{
	if (GameLanguage("Russian")) MprintfiW(L"Оповещение модуля D3D9Wnd", L"Привет, и спасибо, что пользуетесь опцией мультимониторного отображения игры. Я задал разрешение вашей игры на %ux%u. Пожалуйста, перезапустите игру, чтобы подтвердить изменения.", nWidth, nHeight);
	else MprintfiA("D3D9Wnd information", "Hello, and thanks for using the Multi-Monitor feature of D3D9Wnd. I have set your game resolution to %ux%u. Please restart the game to confirm the changes.", nWidth, nHeight);
}

void M_LowBPP(UINT dwDepth)
{
	if (GameLanguage("Russian")) MprintfeW(L"Ошибка модуля D3D9Wnd", L"Извините, но глубина цвета в вашей системе (%u битов на пиксель) слишком низкая для того, чтобы использовать D3D9Wnd. Пожалуйста, задайте глубину цвета хотя бы на 32 бита на пиксель.", dwDepth);
	else MprintfeA("D3D9Wnd error", "Sorry, but the colour depth of your system (%u bits per pixel) is too low for using D3D9Wnd. Please set colour depth to at least 32 bits per pixel.", dwDepth);
}

void M_UnsupportedFullscreenFrontend(int nWidth, int nHeight, LONG dwErrorCode)
{
	if (GameLanguage("Russian"))
		MprintfW(MB_OK | MB_ICONWARNING | MB_TOPMOST, L"Предупреждение модуля D3D9Wnd",
			L"Внимание: не удалось перевести игру в полноэкранный режим с разрешением %ux%u.\n"
			"Ошибка: %s\n"
			"Возможно, это разрешение не поддерживается вашей системой, монитором или "
			"видеокартой/драйверами. Сейчас произойдёт переключение в оконный режим.\n"
			"\n"
			"Чтобы в будущем избежать подобных ошибок, отключите опции \"Fullscreen\" и "
			"\"FullscreenAlternative\" в разделе [FrontendSettings] файла D3D9Wnd.ini, "
			"либо задайте разрешение меню на поддерживаемое при помощи сторонних модулей "
			"(например, SuperFrontendHD).",
		nWidth, nHeight, DispChangeErrorStrW(dwErrorCode));
	else MprintfA(MB_OK | MB_ICONWARNING | MB_TOPMOST, "D3D9Wnd warning",
			"Warning: failed to set the required screen resolution of %ux%u for frontend.\n"
			"Error: %s\n"
			"It's probably unsupported by your system, monitor or GPU/drivers. Switching to windowed mode.\n"
			"\n"
			"In order to avoid such errors in the future, disable the \"Fullscreen\" and "
			"\"FullscreenAlternative\" options in the [FrontendSettings] section of D3D9Wnd.ini, "
			"or change your front-end resolution using external modules (such as SuperFrontendHD).",
		nWidth, nHeight, DispChangeErrorStrA(dwErrorCode));
}

void M_GoodbyeCustomSize()
{
	if (GameLanguage("Russian"))
		InfoBoxW(
			L"Спасибо за использование опции EnableCustomSize. Эта опция более недоступна в D3D9Wnd, "
			"так как для этого теперь есть модуль SuperFrontendHD. Чтобы запустить ваше меню с "
			"выбранными размерами экрана, пожалуйста скачайте и установите SuperFrontendHD. "
			"Документацию вы сможете найти в прилагающемся архиве, а также на вики-ресурсе "
			"Worms2D.info и пользовательских форумах.\n"
			"\n"
			"Чтобы в будущем избежать подобных оповещений, пожалуйста, задайте EnableCustomSize на 0 "
			"в вашем файле D3D9Wnd.ini.",
			L"Уведомление D3D9Wnd");
	else
		InfoBoxA(
			"Thank you for using the EnableCustomSize option. This option has now been superseded "
			"by SuperFrontendHD, and is thus no longer available in D3D9Wnd. In order to run your "
			"frontend at a custom size, please download and install SuperFrontendHD. Documentation "
			"can be found in the download archive, as well as the Worms2D.info Wiki and User Forums.\n"
			"\n"
			"In order to avoid getting these notices in the future, please set EnableCustomSize to 0 "
			"in your D3D9Wnd.ini.",
			"D3D9Wnd notice");
}
