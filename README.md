# D3D9Wnd

This WormKit module allows you to run Worms Armageddon v3.7.x and some other apps in windowed mode, provided the Direct3D 9 rendering API is in use. For more information, check its [wiki page](https://worms2d.info/D3D9Wnd) and the associated user forums.

There are several editions of D3D9Wnd. This is the main edition of the module that includes the whole range of features for Worms Armageddon.

## Building

You need any version of Visual Studio from 2010 to 2017 with the C++ runtime installed (although only 2017 has been tested as of late).
* The "Release" configuration is the recommended way to build the module.
* Use "R_log" if you want the module to create a log at *d3d9wnd.log* in the game's directory. Logs are also created with the "Debug" configuration.
* The "dynamic" configurations are meant to build the module where MinHook is dynamically linked (via a DLL) rather than a static library. This is not recommended, because not only does the DLL have to be present, but also any other modules linking to the same DLL may experience hooking errors (because the standard MinHook allows only one hook to be installed at the same place at a time). Eventually, if MultiHook proves usable, this may no longer be an issue and become the default configuration.

## Installation

- Make sure that "Load WormKit modules" and the "Direct3D 9" Graphics API (any variant) are enabled in the Advanced options of the game.
- Download or build the module.
- Place the DLL, as well as the INI file (if necessary), into the game's directory.
- Your game will now run in windowed mode. If you wish to expand the frontend window to your native resolution, consider also installing [SuperFrontendHD](https://worms2d.info/SuperFrontendHD). Frontend menus can't have a border or be moved around, see below.

## Features

- **Multi-Monitor support**: run the game spanned across several monitors, as long as the monitor setup is in "Extended" mode and not "Duplicated". With *AutoSetResolution* (enabled by default), the required total screen resolution will be automatically set when you run the game.
- **Mouse pinning/unpinning**: you can press Ctrl+G in-game to release the mouse cursor capture from the window. Useful to switch to other windows without Alt+tabbing, and to move the game window (when border is enabled).
- **Super-resolution**: you can run the game at any screen resolution, even those much higher than the maximum of your monitor. For example, you could play or record the game in 4K while on a 1080P monitor. The maximum is your GPU's maximum texture size (common maximums: 4096×4096, 8192×8192, 16384×16384; much older GPUs may be limited to just 2048×2048). Note, however, that the display ratio is not respected in such a mode (and is always expanded to your desktop), and it is your own responsibility to preserve it while setting your custom width and height. Custom resolutions can be set either via the registry or by using [wkLobbyCmd](https://worms2d.info/wkLobbyCmd).
- **Run in background**: the game will continue rendering while it is in background, both in the frontend menus and during gameplay. Useful for streamers, and for multitasking in general, especially on several monitors.
- **Play sounds in background**: continue playing sounds even when the game is in background (but not minimized). This is optional (enabled by default).
- **Window Border and Quick Information**: you can run the game in either borderless or bordered windowed mode during gameplay, and some extended information about the game can be displayed in the window caption.
- **Alternative fullscreen options**: you can run the frontend menus in "Alternative Fullscreen" mode. In that mode, it still technically is windowed, however runs with the display resolution automatically changed using system APIs. This resolves several issues linked to the "vanilla" fullscreen, such as multi-monitor screen flickering, and frontend screens getting partially or completely white. You can also enable the vanilla Fullscreen in both the frontend and in-game. Enabling it in-game will disable all extended in-game features (such as mouse cursor unpinning), but may result in a more responsive experience in rope-related schemes.
- **Fancy startup**: the game can be set to start translucent, progressively becoming opaque. The speed of the animation depends on the frame rate.
- **Static Window Class**: normally, the Window Class of the game window is always random. Using this option, it will never change. Useful for streaming software such as OBS, to preserve window capture when the game is closed and then reopened.
- **Frontend sizing and positioning** (deprecated): an option to stretch the frontend to any specified size and optionally position it in the middle of the screen. This imposes several restrictions: for example, the inability to move the cursor out of the game window (due to coordinate errors, linked with the game's frontend design). These options are now deprecated in favor of [SuperFrontendHD](https://worms2d.info/SuperFrontendHD), which provides native scaling of frontend dialogs with high-resolution graphics and controls.

## Known problems

- Due to the deficient design of DXMFC (the technology behind the game's frontend menus), several of its bugs remain unpatched by the module. For example, the frontend window is stuck to the top-left corner without a border. This is due to how the coordinate system works, producing rendering/input errors when not positioned there. Given D3D9Wnd's scope, developing such patches would bear a lot of burden both on code and development, and still not produce perfect results. Some of them are within the scope of SuperFrontendHD. Many of these bugs are set to be resolved in future updates of the game.
- Some broadcasting software may experience loss of link when gameplay starts or finishes (e.g. OBS in Game Capture mode). This is due to how Direct3D 9 device updates are performed (by using Release instead of Reset). This problem is set to be resolved in future updates of the game.

## Thanks

- **Kawoosh** – for creating the initial version of the module.
- **Deadcode** – for helping resolve some issues related to bugs of the game, such as the Win+L crash.
- Everyone who has reported issues and suggestions over the course of the module's existence.
