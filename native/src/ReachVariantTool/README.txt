
ReachVariantTool by DavidJCobb

--

A GUI tool for editing settings in game variants. It does not offer access to 
Megalo script data at this time (aside from the values of script-defined options). 
The tool can load Multiplayer and Forge variants, but not Firefight or Campaign 
variants. (I'm not sure that Campaign variants are even used in MCC.)

The tool was built primarily for working with Master Chief Collection-era files. 
It should hopefully work well enough with Xbox 360-era files (I've tested a few).

Your saved game variants for MCC should be located in the following folder:

   %APPDATA%\..\LocalLow\MCC\LocalFiles\<YOUR_XUID_HERE>\HaloReach\GameType

where "<YOUR_XUID_HERE>" is replaced by your XUID (your internal Xbox User ID). 
If that folder path isn't present, try saving a game variant in-game so that the 
path gets created.

Note that MCC only checks what game variants you have saved once, when you click 
through the title screen ("Press [Enter]"). If you add any files to the folder 
after that, you'll have to fully close and restart MCC for those files to show 
up in-game.

The built-in game variants should be stored in two folders within the Master Chief 
Collection install directory; the first folder holds built-in variants for use in 
Custom Games, while the second folder holds all Matchmaking game variants:

   ...\steamapps\common\Halo The Master Chief Collection\haloreach\game_variants\
   ...\steamapps\common\Halo The Master Chief Collection\haloreach\hopper_game_variants\

So you can use those as a base for your edits -- just make sure you don't overwrite 
them by accident.


== Scattered usage notes ==

Although Halo: Reach doesn't allow you to save any settings you've changed in the 
Forge lobby, it does support loading them.


== Licensing ==

The bulk of ReachVariantTool is licensed under the GNU Public License version 3. The 
help manual is licensed under the Creative Commons 0 License, which is effectively 
public domain.

This program uses the Qt library, version 5.15.2, per the terms of LGPLv3. The source 
code to Qt is available on their website; per the license terms, I am legally obligated 
to provide you with a copy of that source code on request, in the event that their 
website vanishes. The source code is an 800MB ZIP file (qt-everywhere-src-5.15.2.zip) 
and is as of this writing available here: <http://download.qt.io/archive/qt/5.15/5.15.2/>

zlib is used under the terms of the license in the zlib.h source file.

All associated licenses are included in text within the "LICENSES" folder.


=== Original game assets ===

Halo: Reach © Microsoft Corporation. This program was created under Microsoft's 
"Game Content Usage Rules" using assets from Halo: Reach, and it is not endorsed 
by or affiliated with Microsoft.

List of assets used:

 - Gametype icons (ripped via screenshot)
 - In-game HUD (via screenshots)