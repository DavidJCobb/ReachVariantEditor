# ReachVariantTool
 
A tool for editing Halo: Reach game variants, tested on files obtained from Halo: The Master Chief Collection's PC release.

You can download the program [from NexusMods](https://www.nexusmods.com/halothemasterchiefcollection/mods/192/).

This tool was built using Visual Studio 2019. It dynamically links Qt, which was licensed under LGPLv3, and it incorporates some Halo: Reach gametype icons per Microsoft's Game Content Usage Rules (see README.txt for further attribution). Zlib is used as well; see zlib.h for its license terms.

The repo has a few top-level folders: the "html" folder was for some prototyping I did in JavaScript, to quickly get a handle on reading Reach's bit-aligned data; the "native" folder is for the main program. The former folder contains the JavaScript zlib port "pako," the original portions of which are MIT-licensed (it also incorporates zlib code).

## Build environment

This program was built using Microsoft Visual Studio Community 2019 with the [Qt Visual Studio Tools](https://marketplace.visualstudio.com/items?itemName=TheQtCompany.QtVisualStudioTools2019) plug-in. That plug-in is [GPL-licensed with a special exemption](https://marketplace.visualstudio.com/items/TheQtCompany.QtVisualStudioTools2019/license) which allows its use in developing non-GPL software. Qt Designer was also used to build this program, with [a similar exception](https://opensource.stackexchange.com/questions/7709/using-qt-designer-to-create-ui-design-for-closed-source-application). The [version of Qt used](https://doc.qt.io/qtvstools/qtvstools-managing-projects.html#managing-qt-versions) was 5.12.5, 64-bit, for MSVC 2017.

## License

Qt and its components have their own license; as does zlib; as do some of the game assets used per Microsoft's Game Content Usage Rules. My code is licensed under [Creative Commons CC-BY-NC 4.0](https://creativecommons.org/licenses/by-nc-sa/4.0/) unless otherwise stated (some files are licensed under CC0 i.e. public domain, etc.).