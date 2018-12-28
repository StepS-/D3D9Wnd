# D3D9Wnd

This WormKit module allows you to run Worms Armageddon v3.7.x and some other apps in windowed mode, provided the Direct3D 9 rendering API is in use. For more information, check its [wiki page](https://worms2d.info/D3D9Wnd) and the associated user forums.

There are several editions of D3D9Wnd. This is the WWPR edition of the module, optimized for Worms World Party Remastered. At this moment, it is very lightweight and doesn't include many of the main edition's features. No window border or key shortcuts are available.

## Building

You need any version of Visual Studio from 2010 to 2017 with the C++ runtime installed (although only 2017 has been tested as of late). The "Release" configuration is the recommended way to build the module.

## Installation

- Download [WormKitDS](https://worms2d.info/WormKitDS) and place it into the game's directory to get D3D9Wnd to load.
- Download or build the module.
- Place the DLL into the game's directory.
- The game will now run in borderless windowed mode.
