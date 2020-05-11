# ReachVariantTool
 
A tool for editing Halo: Reach game variants, tested on files obtained from Halo: The Master Chief Collection's PC release.

You can download the program [from NexusMods](https://www.nexusmods.com/halothemasterchiefcollection/mods/192/).

This tool was built using Visual Studio 2019. It dynamically links Qt, which was licensed under LGPLv3, and it incorporates some Halo: Reach gametype icons per Microsoft's Game Content Usage Rules (see README.txt for further attribution). Zlib is used as well; see zlib.h for its license terms.

The repo has a few top-level folders: the "html" folder was for some prototyping I did in JavaScript, to quickly get a handle on reading Reach's bit-aligned data; the "native" folder is for the main program.

## Build environment

This program was built using Microsoft Visual Studio Community 2019 with the [Qt Visual Studio Tools](https://marketplace.visualstudio.com/items?itemName=TheQtCompany.QtVisualStudioTools2019) plug-in. That plug-in is [GPL-licensed with a special exemption](https://marketplace.visualstudio.com/items/TheQtCompany.QtVisualStudioTools2019/license) which allows its use in developing non-GPL software. Qt Designer was also used to build this program, with [a similar exception](https://opensource.stackexchange.com/questions/7709/using-qt-designer-to-create-ui-design-for-closed-source-application).

