# D3D9Wnd

This WormKit module allows you to run Worms Armageddon v3.7.x and some other apps in windowed mode, provided the Direct3D 9 rendering API is in use. For more information, check its [wiki page](https://worms2d.info/D3D9Wnd) and the associated user forums.

There are several editions of D3D9Wnd. This is the "naked" edition of the module that does nothing more than set the "Windowed" flag in Direct3D 9. As such, it should work with any Direct3D 9-based application.

## Building

You need any version of Visual Studio from 2010 to 2017 with the C++ runtime installed (although only 2017 has been tested as of late). The "Release" configuration is the recommended way to build the module.

## Installation

- Make sure that your application supports the Direct3D 9 rendering API and that it can load external DLL files. You can get [WormKitDS](https://worms2d.info/WormKitDS) to get the module to load in a DirectX application.
- Download or build the module.
- Place the DLL into the game's directory.
- Your application will now run in borderless windowed mode.
