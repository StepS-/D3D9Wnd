
#include "hooks_win.h"
#include "hooks_api.h"
#include "tools\tools.h"
#include "misc.h"

HHOOK hkb, hkb2, hkb3;

LRESULT CALLBACK KeyboardProc(int nCode, WPARAM wKeyCode, LPARAM lParam)
{
	if (HC_ACTION == nCode && !!!(lParam & INT_MIN))
	{
		if (InGame())
		{
			if (KeyPressed(VK_CONTROL))
			{
				switch (wKeyCode)
				{
				case 'G': // G - mouse unpinning
					if (ShowCursorCount() < 0)
					{
						ShowWindow(WA.Wnd.W2D, SW_HIDE);
						SetForegroundWindow(FindWindow("Shell_TrayWnd", NULL));
						//workaround: WA must lose focus at least once in the current session for cursor unsnapping to take effect.

						ShowCursorN(0);
					}

					else
					{
						ShowWindow(WA.Wnd.W2D, SW_SHOW);
						ShowCursorN(-1);
					}

					qFileLog("KeyboardProc: Switched cursor visibility via Ctrl+G.");

					UpdateWACaption();
					break;


				case 'T': // T - topmost
					ChangeTopmostState();
					fFileLog("KeyboardProc: Switch to %s triggered via keyboard.", Settings.Misc.NoTopmost ? "no topmost" : "topmost");
					break;

				case 'H': // H - active background
					if (ToggleActiveBackground(!Settings.IG.Background))
						fFileLog("KeyboardProc: Toggled Active Background %s with Ctrl+H.", Settings.IG.Background ? "ON" : "OFF");
					else
						qFileLog("KeyboardProc: Attempted to toggle active background with Ctrl+H, but not done anything since it's forcibly enabled with DWM Desktop Composition disabled.");
					break;

				case 'D': // D - quick window border :)
					if (!Settings.IG.Stretch || (WA.BB.Width < Env.Act.ResX || WA.BB.Height < Env.Act.ResY))
					{
						//TODO: simplify these things
						if (Settings.IG.WindowBorder)
						{
							RECT ClRect, RClRect;
							GetClientRect(WA.Wnd.DX, &ClRect);
							GetClientRect(WA.Wnd.DX, &RClRect);
							ClientToScreen(WA.Wnd.DX, (POINT*)&RClRect);
							SetWindowLong(WA.Wnd.DX, GWL_STYLE, GetWindowLong(WA.Wnd.DX, GWL_STYLE) &~WS_CAPTION);
							SetWindowPos(WA.Wnd.DX, HWND_TOP, RClRect.left, RClRect.top, ClRect.right - ClRect.left, ClRect.bottom - ClRect.top, SWP_NOZORDER);
							if (Settings.IG.QuickInfo) SetWindowText(WA.Wnd.DX, "Worms Armageddon (windowed)");
							Settings.IG.WindowBorder = 0;
						}

						else
						{
							RECT ClRect, RClRect, WRect;
							GetClientRect(WA.Wnd.DX, &ClRect);

							if (ClRect.right - ClRect.left < Env.Act.ResX && ClRect.bottom - ClRect.top < Env.Act.ResY)
							{
								DWORD Style = GetWindowLong(WA.Wnd.DX, GWL_STYLE) | WS_CAPTION;
								SetWindowLong(WA.Wnd.DX, GWL_STYLE, Style);
								SetWindowLong(WA.Wnd.DX, GWL_STYLE, GetWindowLong(WA.Wnd.DX, GWL_STYLE) &~WS_MAXIMIZEBOX);
								AdjustWindowRect(&ClRect, Style, 0);
								SetWindowPos(WA.Wnd.DX, HWND_TOP, 0, 0, ClRect.right - ClRect.left, ClRect.bottom - ClRect.top, SWP_NOMOVE | SWP_NOZORDER);
								GetWindowRect(WA.Wnd.DX, &WRect);
								GetClientRect(WA.Wnd.DX, &RClRect);
								ClientToScreen(WA.Wnd.DX, (POINT*)&RClRect);
								SetWindowPos(WA.Wnd.DX, HWND_TOP, WRect.left - (RClRect.left - WRect.left), WRect.top - (RClRect.top - WRect.top), 0, 0, SWP_NOSIZE | SWP_NOZORDER);
								Settings.IG.WindowBorder = 1;
								UpdateWACaption();
							}
						}

						fFileLog("KeyboardProc: Toggled the window border %s with Ctrl+G.", Settings.IG.WindowBorder ? "ON" : "OFF");
					}
					else
						qFileLog("KeyboardProc: Attempted to toggle the window border with Ctrl+G, but not done anything since the game's window is too big for it.");
					break;

				case 'L': // L - lefttop
					if (!Settings.IG.Stretch || (WA.BB.Width < Env.Sys.PrimResX || WA.BB.Height < Env.Sys.PrimResY))
					{
						if (!Settings.IG.TopLeftPosition)
						{
							SetWindowPos(WA.Wnd.DX, NULL, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
							if (!Settings.IG.Background) StickW2Wnd();
							Settings.IG.TopLeftPosition = true;
						}

						else if (Settings.IG.TopLeftPosition)
						{
							RECT WArect;
							GetWindowRect(WA.Wnd.DX, &WArect);
							int newx = (Env.Act.ResX / 2) - (WArect.right - WArect.left) / 2;
							int newy = (Env.Act.ResY / 2) - (WArect.bottom - WArect.top) / 2;
							SetWindowPos(WA.Wnd.DX, NULL, newx, newy, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
							if (!Settings.IG.Background) StickW2Wnd();
							Settings.IG.TopLeftPosition = false;
						}

						fFileLog("KeyboardProc: Toggled the top-left window position with Ctrl+L to %s.", Settings.IG.TopLeftPosition ? "ON" : "OFF");
					}
					else
						qFileLog("KeyboardProc: Attempted to toggle the top-left window position with Ctrl+L, but not done anything since the game window covers entire screen.");
					break;
				}
			}
		}

		else if (KeyPressed(VK_CONTROL))
		{
			if (KeyPressed(VK_SHIFT))
			{
				if (wKeyCode == VK_F12)
				{
					Env.EasterEgg.InControl = !Env.EasterEgg.InControl;
					fFileLog("Easter egg toggled %s!", Env.EasterEgg.InControl ? "ON" : "OFF");
				}
				
				else if (Env.EasterEgg.InControl)
				{
					RECT WndRect;
					GetWindowRect(WA.Wnd.MFC, &WndRect);
					int xpos = WndRect.left, ypos = WndRect.top;

					if (wKeyCode >= '1' && wKeyCode <= '9')
						Env.EasterEgg.Multiplier = wKeyCode - 0x30;

					else if (wKeyCode >= VK_LEFT && wKeyCode <= VK_DOWN)
						SetWindowPos(WA.Wnd.MFC, NULL,
						xpos + Env.EasterEgg.Multiplier * (wKeyCode % 2)       * (-VK_UP   + wKeyCode),
						ypos + Env.EasterEgg.Multiplier * ((wKeyCode + 1) % 2) * (-VK_RIGHT + wKeyCode),
						0, 0, SWP_NOSIZE | SWP_NOZORDER);

					else if (wKeyCode == 'Z')
					{
						if (!Env.EasterEgg.FrontendHidden)
							SetWindowPos(WA.Wnd.MFC, NULL, -32000, -32000, 0, 0, SWP_NOSIZE | SWP_NOZORDER); //lol
						else
							SetWindowPos(WA.Wnd.MFC, NULL, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOZORDER);

						Env.EasterEgg.FrontendHidden = !Env.EasterEgg.FrontendHidden;
					}
				}
			}

			else if (wKeyCode == 'T')
				ChangeTopmostState();
		}
	}
	return CallNextHookEx(hkb, nCode, wKeyCode, lParam);
}

LRESULT CALLBACK CallWndProc(int nCode, WPARAM wParam, LPARAM lParam)
{
	if (nCode == HC_ACTION)
	{
		CWPSTRUCT* pwp = (CWPSTRUCT*)lParam;

		if (pwp->hwnd == WA.Wnd.DX)
		{
			switch (pwp->message)
			{
			case  WM_ACTIVATEAPP:
				if (InGame())
				{
					if (Settings.IG.AutoUnpin)
					{
						if (!pwp->wParam)
						{
							ShowWindow(WA.Wnd.W2D, SW_HIDE);

							ShowCursorN(0);

							qFileLog("WindowProc: Lost focus and auto-unpinned the cursor.");
						}

						else if (Settings.IG.Stretch || (WA.BB.Width >= Env.Sys.PrimResX || WA.BB.Height >= Env.Sys.PrimResY))
						{
							ShowWindow(WA.Wnd.W2D, SW_SHOW);
							ShowCursorN(-1);

							qFileLog("WindowProc: Regained focus and pinned the cursor back automatically.");
						}

						UpdateWACaption();
					}
					
					if (pwp->wParam)
					{
						SendMessage(WA.Wnd.W2D, WM_KEYUP, VK_MENU, 0); //Workaround: send WM_KEYUP to Alt.
						SetCursorPos(WA.BB.Width / 2, WA.BB.Height / 2);
						//TODO: needs rethinking of a proper way to do it. This SetCursorPos thing results in a cursor jump when clicking caption in bordered window mode.
					}
				}
				else if (Settings.FR.AltFullscreen)
				{
					if (!pwp->wParam)
					{
						SetScreenRes(Env.Sys.PrimResX, Env.Sys.PrimResY);
						qFileLog("WindowProc: Lost focus in artificial fullscreen frontend and changed the screen resolution back to desktop.");
					}
					else
					{
						SetScreenRes(WA.BB.Width, WA.BB.Height);
						qFileLog("WindowProc: Regained focus in artificial fullscreen frontend and changed the screen resolution back to frontend resolution.");
					}
				}
				break;

			case WM_EXITSIZEMOVE:
				if (!Settings.IG.Background && !Settings.IG.Stretch) StickW2Wnd();
				break;

			case WM_DWMCOMPOSITIONCHANGED:
				if ((Env.Sys.DwmEnabled = DWMEnabled()) == 0)
				{
					if (!Settings.IG.Background)
					{
						ToggleActiveBackground(TRUE);
						qFileLog("WindowProc: DWM composition switched to OFF. Active background has been enabled.");
					}
					else
						qFileLog("WindowProc: DWM composition switched to OFF.");
				}
				else
					qFileLog("WindowProc: DWM composition switched to ON.");
				break;
			}
		}
	}

	return CallNextHookEx(hkb2, nCode, wParam, lParam);
}

LRESULT CALLBACK MouseProc(int nCode, WPARAM wParam, LPARAM lParam)
{
	if (nCode == HC_ACTION)
	{
		MOUSEHOOKSTRUCT* pwp = (MOUSEHOOKSTRUCT*)lParam;
		if (pwp->hwnd == WA.Wnd.DX)
		{
			if (InGame())
			{
				if (wParam == WM_LBUTTONDOWN && !IsWindowVisible(WA.Wnd.W2D))
				{
					ShowCursorN(-1);
					SetCursorPos(WA.BB.Width / 2, WA.BB.Height / 2);
					ShowWindow(WA.Wnd.W2D, SW_SHOW);

					qFileLog("MouseProc: Cursor pinned back to the game through user click.");

					UpdateWACaption();
				}
			}
		}
	}

	return CallNextHookEx(hkb3, nCode, wParam, lParam);
}
