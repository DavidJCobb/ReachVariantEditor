#include "./attach_to_parent_console.h"
#include <stdio.h>
#include <iostream>
#include <Windows.h>

// Control whether we attempt to use `dup2`/`_dup2` to redirect the C IO streams 
// to Windows console handles.
//
// This doesn't work, because the C IO streams don't exist when the program is 
// compiled as a Windows application; they're never opened. That means that they 
// don't have underlying file descriptors for `dup2` to remap. This approach is 
// mainly retained as documentation of what *doesn't* work.
#define USE_DUP2 0

#if USE_DUP2
   #include <fcntl.h> // _O_TEXT
   #include <io.h> // _open_osfhandle

   // A variation on the approach recommended by Raymond Chen on The Old New Thing:
   // https://devblogs.microsoft.com/oldnewthing/20180221-00/?p=98065
   // "How can I call freopen but open the file with shared access instead of exclusive access?"
   // (Feb. 21, 2018)
   //
   // Chen's code closes the file descriptor after remapping, because he opens an 
   // actual file and redirects output to that. The `_close` function closes both 
   // the file descriptor and the underlying OS handle, which we extremely don't 
   // want to do to the console handles.
   static void reassign_stream_to_handle(auto stream, HANDLE handle, int mode = _O_WRONLY | _O_TEXT) {
      if (handle == INVALID_HANDLE_VALUE)
         return;
      int file_descriptor = _open_osfhandle((intptr_t)handle, mode);
      if (file_descriptor == -1)
         return;
      int original_stream_file_descriptor = _fileno(stream);
      switch (original_stream_file_descriptor) {
         case -1: // error
         case -2: // there was no output stream to begin with
            return;
      }
      _dup2(file_descriptor, original_stream_file_descriptor);
      setvbuf(stream, NULL, _IONBF, 0);
   }
#endif

static void attach_to_current_console() {
   #if USE_DUP2
      reassign_stream_to_handle(stdout, GetStdHandle(STD_OUTPUT_HANDLE), _O_WRONLY | _O_TEXT);
      reassign_stream_to_handle(stderr, GetStdHandle(STD_ERROR_HANDLE),  _O_WRONLY | _O_TEXT);
      reassign_stream_to_handle(stdin,  GetStdHandle(STD_INPUT_HANDLE),  _O_RDONLY | _O_TEXT);
   #else
      // Per MS documentation, these sentinel strings can be used as "filenames" that refer 
      // to the console handles.
      // https://learn.microsoft.com/en-us/windows/console/console-handles
      freopen("CONOUT$", "w", stdout);
      freopen("CONOUT$", "w", stderr);
      freopen("CONIN$",  "r", stdin);
   #endif

   // Synchronize C++ streams to C streams (i.e. std::cout to stdout and so on):
   std::ios_base::sync_with_stdio(true);
}

extern attach_to_console_result attach_to_parent_console() {
   #if _WIN32_WINNT < 0x0502 // AttachConsole min support per docs
      return attach_to_console_result::not_supported;
   #else
      auto result = AttachConsole(ATTACH_PARENT_PROCESS);
      if (result) {
         attach_to_current_console();
         return attach_to_console_result::success;
      }
      switch (GetLastError()) {
         case ERROR_ACCESS_DENIED:
            return attach_to_console_result::already_have_a_console;
         case ERROR_INVALID_HANDLE:
            return attach_to_console_result::parent_process_has_no_console;
         case ERROR_INVALID_PARAMETER:
            return attach_to_console_result::no_parent_process;
      }
   #endif
}
extern bool attach_to_new_console() {
   auto result = AllocConsole();
   if (result) {
      attach_to_current_console();
      return true;
   }
   return false;
}