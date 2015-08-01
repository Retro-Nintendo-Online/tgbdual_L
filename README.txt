TGB Dual L
http://github.com/libertyernie/tgbdual_L

Based on TGB Dual Kai from http://blog.gameboymania.com/2014/03/tgbdualkaiv1141.html

TGB Dual (originally written by Hii in 2000–2004) is an open-source Game Boy
Color emulator with link cable support.

TGB Dual makes it possible to emulate two Game Boy consoles at once (Slot 1
and Slot 2), using two different ROMs and save files. Opening a ROM in Slot 2
will open a new window. The two slots have different button mappings; you can
check or change them with Options > Keys.

My branch of TGB Dual was originally based on the libretro port, but since
libretro has removed the Win32 code from its repository, and because TGB Dual
Kai has Win32-specific features, it makes more sense to use this as the base.

New features (compared to TGB Dual Kai):
  * TGB Dual can now load ROMs built with Goomba and Goomba Color (GB emulators
    for the GBA), and can read and write to those emulators' save files. This
    is helpful if you want to share your save data between PC and GBA, because
    the ROMs run faster in a GB emulator than they would in a GBA emulator
    trying to run Goomba. (Also, you can use link cable emulation for games
    like Pokémon.) See goomba.txt for more information.
  * Supported file extensions added: .gba (for Goomba/Goomba Color) and .sgb.
* Windows version only:
  * All dialogs/menus have been translated to English at the source code level
    ("english" branch in git.)
  * Along with the ROM and SRAM folders, users can now specify the SRAM file
    extensions used for Slot1 and Slot2 (the defaults are still .sav and .sa2.)
  * The options for showing the FPS meter are now Off, Slot1, Slot2, and Both.

Important code changes:
* Previously, the files in win32_ui were encoded in Shift-JIS, but this did
  not compile properly on non-Japanese computers. Now, the source code has
  been changed to UTF-8 for storage in the Git repository, and is converted to
  UTF-16 when it is compiled, then back to UTF-8 when using Clean in Visual
  Studio.
  * This automatic conversion is *only* set up in Visual Studio 2013
    (TGB_Dual (VS2013).sln), and it uses the endian_conv.exe helper app.
* The VS2013 project also defines the preprocessor macro TGBDUAL_USE_DINPUT8,
  so dx_renderer.h and settings.cpp will use dinput8.lib (DirectInput 8).
