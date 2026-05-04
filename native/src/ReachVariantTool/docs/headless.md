
# Headless

ReachVariantTool can be run headlessly, i.e. without ever spawning a GUI. This presents some challenges for development.

## The problem with optionally-headless apps

Windows allows you to compile programs to run on either the Windows subsystem or the Console subsystem. This influences how the programs themselves behave, and how a console behaves when directed to execute them.

* A Windows-subsystem program is intended to have a GUI. If this program is run via a console window, the console won't wait for the prorgam to finish execution; the console window will continue onward (e.g. to the next command, if in a batch file, or to prompting the user for further input). Additionally, the Windows-subsystem program won't inherit console handles (i.e. for input and output) and thus won't ever open any I/O streams; the C-era stream accessors `stdout`, `stderr`, and `stdin` won't have an underlying stream to wrap.

* A Console-subsystem program is intended not to have a GUI. If this program is run via a console window, the console will wait for the program to finish executing. Additionally, the Console-subsystem program will inherit console handles, which can be written to via `stdout` and friends. However, if a Console-subsystem program is run via the Windows GUI, it will spawn its own console window.

Of course, a Windows-subsystem program is allowed to attach itself to a console, and a Console-subsystem program is allowed to spawn GUI objects. The distinction lies mainly in the program's relationship to a console used to spawn it. This means that if you have a GUI program with a headless mode, you can compile that program for either subsystem, and use different workarounds to support both modes of execution.


## Solutions

### Compiling as a GUI application

A Windows-subsystem program that is executed from a console window can attach itself to that console window using `AttachConsole(ATTACH_PARENT_PROCESS)`. With some trickery, it can take the empty/null C-era stream accessors and repoint them to that console window's handles. However, this still leads to a substandard experience when running the program via a console, because the following is what happens:

1. You use the console to execute the program.
2. The program begins executing.
3. The console prompts you for your next command.
4. The program begins writing output to the console, between the displayed command prompt and anything you type.

That looks like this:

```text
C:/YourStuff/>MyProgram.exe --options
C:/YourStuff/>THIS IS PROGRAM OUTPUT
THIS IS MORE PROGRAM OUTPUT

and the things you type next end up down here
```

This, frankly, sucks.

#### Side note: how do you actually make this work?

Once you're attached to a console, you can make these calls to redirect the empty C stream accessors to the current console handles:

```c++
freopen("CONOUT$", "w", stdout);
freopen("CONOUT$", "w", stderr);
freopen("CONIN$",  "r", stdin);

// Synchronize C++ streams to C streams (i.e. std::cout to stdout and so on):
std::ios_base::sync_with_stdio(true);
```

[`freopen`](https://learn.microsoft.com/en-us/cpp/c-runtime-library/reference/freopen-wfreopen?view=msvc-170) is a POSIX API that takes a `FILE*` and remaps it, given a file path and mode. It's intended to work on, like, *actual files*, but evidently you *can* pass `stdout` and friends into it.

It's very fortunate that Microsoft provided sentinel filenames that access the console handles, as documented [here](https://learn.microsoft.com/en-us/windows/console/console-handles). The only way to remap a `FILE*` (or `FILE*`-like object) *without* using a filename is by using `_open_osfhandle` to convert the console output handle[^console-output-handle] handle into a file descriptor, using `_fileno(stdout)` to get `stdout`'s current file descriptor, and then using `dup2` to remap the latter descriptor so that it acts as an alias to the former descriptor. Raymond Chen has documented the basic approach[^chen-doesnt-close] of remapping a `FILE*` to a `HANDLE` [on The Old New Thing](https://devblogs.microsoft.com/oldnewthing/20180221-00/?p=98065).[^chen] However, a Windows-subsystem program has empty C stream accessors: `stdout` and friends *have* no file descriptors to remap; `_fileno(stdout)` [produces the sentinel value -2](https://learn.microsoft.com/en-us/cpp/c-runtime-library/reference/fileno?view=msvc-170) because `stdout` is not currently associated with an output stream. Your only option is `freopen` using the sentinel filenames, as shown above.

[^console-output-handle]: `GetStdHandle(STD_OUTPUT_HANDLE)`

[^chen]: Since Microsoft likes to completely break all inbound links to their devblogs every few years, here's a more durable citation: "How can I call freopen but open the file with shared access instead of exclusive access?" by Raymond Chen, via The Old New Thing (Feb. 21, 2018).

[^chen-doesnt-close]: In the sample code he gives, Chen calls `_close` on the throwaway file descriptor that he creates from an OS handle. You should avoid doing this for the purposes we discuss here, as `_close` closes both the descriptor and any underlying OS handle.

### Compiling as a console application

A Console-subsystem program that is executed from the Windows GUI (e.g. taskbar, Explorer) will have a console window spawn. It can dismiss that console window immediately by calling `FreeConsole()`; this detaches the program from that console, and since nothing else is attached to the console, the window will de-spawn immediately. However, even the fastest-starting programs will still have the console window be visible for a split-second.

Aside from that annoyance, though, everything works properly.


## In source code

When you create a project in Visual Studio, you have to select whether you're creating a Windows Application or a Console Application. This sets the linker subsystem, and separately from that, it configures the compiler to define an appropriate macro (`_WINDOWS` or `_CONSOLE`) that your source code can check. **Nothing keeps these two settings in sync.** If you change the linker subsystem, that affects the behaviors described above, but it does not automatically update the macros supplied to your source code. You have to manually edit your project settings to change what macros are defined.

Worse, Qt VS Tools appears to forcibly define the `_WINDOWS` macro, and I cannot find where or how it does this. Even if you clear that macro out of your project settings, and even if you configure your project settings to define `_CONSOLE`, the `_WINDOWS` macro gets defined as well. This further complicates checking what you've been compiled for.


## RVT's approach

Compiling as a console application produces a minor and potentially annoying hiccup, but otherwise leads to correct behavior. Compiling as a Windows application avoids the hiccup, but results in drastically wrong output when run via a terminal.

Thus, I'm going with the former option.
