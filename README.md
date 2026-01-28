# ReachVariantTool
 
A tool for editing Halo: Reach game variants, tested on files obtained from Halo: The Master Chief Collection's PC release.

You can download the program [from NexusMods](https://www.nexusmods.com/halothemasterchiefcollection/mods/192/).

The repo has a few top-level folders. The "html" folder was for some prototyping I did in JavaScript, to quickly get a handle on reading Reach's bit-aligned data, and is provided as a reference for other developers wishing to research or create content for Halo: Reach. The "native" folder is for the main program.

## Contributing

This program was built using Microsoft Visual Studio Community 2022 with the [Qt Visual Studio Tools](https://marketplace.visualstudio.com/items?itemName=TheQtCompany.QtVisualStudioTools2019) plug-in and with Qt Designer. The [version of Qt used](https://doc.qt.io/qtvstools/qtvstools-managing-projects.html#managing-qt-versions) was 5.15.2, 64-bit, for MSVC 2019.

For detailed information on contributing to the main project, please refer to `HOW TO CONTRIBUTE.md`.

## License

Qt and its components are used under the terms of LGPLv3. Zlib has its own license terms, listed in `zlib.h`. Some game assets are used per Microsoft's Game Content Usage Rules (see README.txt for further attribution). The JavaScript zlib port "pako" is used in some research materials included in this repo; the original portions of "pako" are MIT-licensed, and the library also includes zlib code.

The Qt Visual Studio Tools plug-in is [GPL-licensed with a special exemption](https://marketplace.visualstudio.com/items/TheQtCompany.QtVisualStudioTools2019/license) which allows its use in developing non-GPL software. Qt Designer has [a similar exception](https://opensource.stackexchange.com/questions/7709/using-qt-designer-to-create-ui-design-for-closed-source-application).

Prior to September 2021, ReachVariantTool was licensed under [Creative Commons CC-BY-NC 4.0](https://creativecommons.org/licenses/by-nc-sa/4.0/) unless otherwise stated (some files were licensed under CC0).

As of September 2021, ReachVariantTool's program code is provided under the GNU General Public License version 3, while ReachVariantTool's documentation (including source XML files, "rendered" HTML files, and embedded images) is provided under CC0. Some individual source code files from ReachVariantTool are still be provided under CC0, with an appropriate notice at the top of each such file. As with any license change, content which has yet to be modified since the change may legally be used under either the previous license(s) or the current one(s), at the user's discretion.
