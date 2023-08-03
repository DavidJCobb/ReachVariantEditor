
# To use...

1. Build this project. Yes, even though there's no C/C++ code in it. I've modified the project file to run some custom C# code: it'll pull the VC redistributable file version and overwrite `vc-version.iss`.

2. Run the Inno Setup editor/compiler on `main.iss` and build the installer.