#include "./monitor_process_state.h"
#include <format>
#include <iostream>
#include <string_view>
#include <windows.h>
#include <dbghelp.h> // MINIDUMP_EXCEPTION_INFORMATION and friends
#pragma comment(lib, "Dbghelp.lib")
#include "./crash_info.h"
#include "../../is_console_application.h"

// windows header garbage:
#undef small
#undef min
#undef max

#include "../../rvt_version_macros.h"

#define STR(x) L###x
#define XSTR(x) STR(x)

#define PROGRAM_NAME L"ReachVariantTool"

#define VERSION_STRING XSTR(VER_MAJOR) L"." XSTR(VER_MINOR) L"." XSTR(VER_PATCH) L"." XSTR(VER_BUILD)

namespace {
   constexpr const bool only_referenced_modules = false;

   // Equivalent to WER_DUMP_TYPE::WerDumpTypeMiniDump
   constexpr const auto small_minidump_flags = (MINIDUMP_TYPE)(MiniDumpWithDataSegs | MiniDumpWithUnloadedModules | MiniDumpWithProcessThreadData | MiniDumpWithTokenInformation);

   // Equivalent to WER_DUMP_TYPE::WerDumpTypeHeapDump
   constexpr const auto large_minidump_flags = (MINIDUMP_TYPE)(MiniDumpWithDataSegs | MiniDumpWithProcessThreadData | MiniDumpWithHandleData | MiniDumpWithPrivateReadWriteMemory | MiniDumpWithUnloadedModules | MiniDumpWithFullMemoryInfo | MiniDumpWithThreadInfo | MiniDumpWithTokenInformation | MiniDumpWithPrivateWriteCopyMemory);
}

static BOOL _minidump_callback(
   PVOID                            param,
   const PMINIDUMP_CALLBACK_INPUT   input,
   PMINIDUMP_CALLBACK_OUTPUT        output
) {
   switch (input->CallbackType) {
      case ModuleCallback:
         //
         // Omit code segments. We shouldn't need those; we should have the relevant 
         // modules on hand already.
         //
         if constexpr (only_referenced_modules) {
            output->ModuleWriteFlags &= ModuleReferencedByMemory;
            if (output->ModuleWriteFlags) {
               output->ModuleWriteFlags = (
                  ModuleWriteModule |
                  ModuleWriteDataSeg |
                  ModuleReferencedByMemory |
                  ModuleWriteTlsData
               );
            }
         } else {
            output->ModuleWriteFlags = (
               ModuleWriteModule |
               ModuleWriteDataSeg |
               (output->ModuleWriteFlags & ModuleReferencedByMemory) |
               ModuleWriteTlsData
            );
         }
         break;
   }
   return TRUE;
}

static std::wstring get_or_create_minidump_folder() {
   constexpr const std::wstring_view app_name = L"ReachVariantTool";

   std::wstring path;
   path.resize(MAX_PATH);
   size_t size = GetTempPathW(path.size(), path.data());
   if (size > path.size()) {
      path.resize(size);
      GetTempPathW(size, path.data());
   } else if (size < path.size()) {
      path.resize(size);
   }
   if (size && path.back() != L'\\') {
      path += L'\\';
   }
   path += app_name;
   path += L"\\";

   CreateDirectoryW(path.data(), NULL);

   return path;
}
static std::wstring minidump_filename(const SYSTEMTIME& local_time, uint32_t process_id, uint32_t thread_id, bool large) {
   constexpr const std::wstring_view app_version = VERSION_STRING;

   return std::format(
      L"v{:s}-{:04d}{:02d}{:02d}-{:02d}{:02d}{:02d}-PID{:d}-TID{:d}-{:s}.dmp",
      app_version.data(),
      local_time.wYear,
      local_time.wMonth,
      local_time.wDay,
      local_time.wHour,
      local_time.wMinute,
      local_time.wSecond,
      process_id,
      thread_id,
      large ? L"LARGE" : L"SMALL"
   );
}

namespace {
   namespace strings {
      constexpr const std::wstring_view intro =
         PROGRAM_NAME L" has encountered an irrecoverable error (basically, a crash)."
      ;

      constexpr const std::wstring_view cause_generic_terminate =
         L"An internal function called `std::terminate` was called; there's no available information "
         L"as to why."
      ;

      constexpr const std::wstring_view cause_generic_throw =
         L"An error was thrown and not caught, but the error wasn't a normal std::exception, so no "
         L"further information is available."
      ;

      constexpr const std::wstring_view cause_cpp_exception_with_text =
         L"An exception was thrown but not caught. The exception had the following text:"
      ;

      constexpr const std::wstring_view cause_cpp_exception_without_text =
         L"An exception was thrown but not caught. The exception didn't have any text information."
      ;
      
      constexpr const std::wstring_view minidump_success =
         PROGRAM_NAME L" has saved error information into a \"minidump\" file, which you can send "
         L"to the developer to help them investigate the crash. That file is located here:"
      ;

      constexpr const std::wstring_view minidump_failed =
         PROGRAM_NAME L" tried to save error information into a \"minidump\" file. Unfortunately, "
         L"it was unable to save the minidump"
      ;
      
      constexpr const std::wstring_view minidump_how_to_copy_one_path =
         L"If you press Ctrl + C while this dialog is open, you should be able to copy its text, to "
         L"get the minidump file path and the other error information."
      ;
      
      constexpr const std::wstring_view copy_and_paste_without_minidump =
         L"(You can press Ctrl + C on this dialog to copy its text. Please do so, so you can "
         L"send as much information to " PROGRAM_NAME L"'s developer as possible.)"
      ;
      
      constexpr const std::wstring_view please_save_copies_of_your_files =
         L"If you were working with any of your own files at the time of the crash, please save a copy "
         L"of those files somewhere too, in case " PROGRAM_NAME L"'s developer asks to look at those "
         L"files' exact data (i.e. at the time of the crash) later on."
      ;
   }
}

namespace rvt::subsystems::crash_dumper {
   monitor_process_state::monitor_process_state() {
      this->pipe.send    = GetStdHandle(STD_OUTPUT_HANDLE);
      this->pipe.receive = GetStdHandle(STD_INPUT_HANDLE);
   }

   void monitor_process_state::main(bool headless) {
      this->headless = headless;
      if (this->pipe.send == INVALID_HANDLE_VALUE || this->pipe.receive == INVALID_HANDLE_VALUE) {
         #if _DEBUG
            if (IsDebuggerPresent()) {
               __debugbreak();
            }
         #endif
         ExitProcess(1);
         return;
      }

      if constexpr (is_console_application) {
         //
         // Prevent the user from bamboozling us by running us with `--crash-handler` from the 
         // terminal.
         //
         if (GetFileType(this->pipe.receive) != FILE_TYPE_PIPE) {
            std::wcout << L"Crash-dumping process failed the handshake. (stdin is not an IPC pipe)\n";
            std::wcout << L"(Did you manually run RVT with the --crash-handler switch? Please don't.)\n";
            ExitProcess(1);
            return;
         }
      }
      //
      // Get a console handle to write to, for if we're invoked by a headless RVT.
      //
      if (!ipc::read_value(this->pipe.receive, this->console.output)) {
         ExitProcess(1);
      }

      //
      // Now, wait for crash info to be sent.
      //

      EXCEPTION_POINTERS ep = { nullptr, nullptr };
      crash_info         info;
      if (!info.read(this->pipe.receive)) {
         return;
      }
      const auto process_id = GetProcessId(info.source.process);

      std::wstring minidump_path;
      bool         minidump_saved = false;

      {
         //
         // Create minidumps.
         //
         MINIDUMP_CALLBACK_INFORMATION callback_info = {
            .CallbackRoutine = &_minidump_callback,
            .CallbackParam   = 0,
         };
         MINIDUMP_EXCEPTION_INFORMATION exception_info = {
            .ThreadId          = info.source.thread_id,
            .ExceptionPointers = &ep,
            .ClientPointers    = FALSE, // use TRUE for an in-process dump; we're doing an out-of-process dump so we use FALSE
         };

         if (auto* casted = std::get_if<crash_types::structured_exception>(&info.data)) {
            if (casted->context.has_value()) {
               ep.ContextRecord = &casted->context.value();
            }
            if (!casted->records.empty()) {
               ep.ExceptionRecord = &casted->records[0];
            }
         }

         auto p_exception_info = &exception_info;
         if (!ep.ContextRecord || !ep.ExceptionRecord)
            p_exception_info = nullptr;

         SYSTEMTIME local_time;
         GetLocalTime(&local_time);

         std::wstring path = get_or_create_minidump_folder();

         minidump_path = path + minidump_filename(local_time, process_id, info.source.thread_id, true);

         auto _try_minidump = [&info, process_id, p_exception_info, &callback_info](HANDLE file, MINIDUMP_TYPE flags) -> bool {
            bool result = MiniDumpWriteDump(info.source.process, process_id, file, flags, p_exception_info, nullptr, &callback_info);
            if (!result) {
               //
               // Minidumps can fail with the error code `ERROR_PARTIAL_COPY`. Conventional wisdom online is that 
               // this... just happens randomly? I've found multiple discussions wherein people worked around the 
               // problem by just having their crash dumpers use a retry loop for that error code specifically.
               //
               size_t tries = 0;
               do {
                  auto error = GetLastError();
                  if (error != ERROR_PARTIAL_COPY)
                     break;
                  result = MiniDumpWriteDump(info.source.process, process_id, file, flags, p_exception_info, nullptr, &callback_info);
               } while (!result && ++tries < 4);
            }
            return result;
         };

         {
            HANDLE dump_file = _try_create_file_for_minidump(minidump_path);
            if (dump_file != INVALID_HANDLE_VALUE) {
               minidump_saved = _try_minidump(dump_file, large_minidump_flags);
               if (!minidump_saved) {
                  #if _DEBUG
                     minidump_saved = false; // put a breakpoint here
                  #endif
               }
               CloseHandle(dump_file);
            }
         }
      }
      
      std::wstring message;
      message += strings::intro;
      if (auto* casted = std::get_if<crash_types::cpp_exception>(&info.data)) {
         if (casted->what.empty()) {
            message += strings::cause_cpp_exception_without_text;
         } else {
            message += strings::cause_cpp_exception_with_text;
            message += L"\n\n";
            message += std::move(casted->what);
         }
      } else if (auto* casted = std::get_if<crash_types::cpp_generic_terminate>(&info.data)) {
         message += strings::cause_generic_terminate;
      } else if (auto* casted = std::get_if<crash_types::cpp_generic_throw>(&info.data)) {
         message += strings::cause_generic_throw;
      } else if (auto* casted = std::get_if<crash_types::structured_exception>(&info.data)) {
         ;
      }
      message += L"\n\n";
      if (minidump_saved) {
         message = strings::minidump_success;
         message += L"\n\n";
         message += minidump_path;
         if (!this->headless) {
            message += L"\n\n";
            message += strings::minidump_how_to_copy_one_path;
         }
      } else {
         message += strings::minidump_failed;
         if (!this->headless) {
            message += L"\n\n";
            message += strings::copy_and_paste_without_minidump;
         }
      }
      message += L" ";
      message += strings::please_save_copies_of_your_files;
      _show_message(message);

      // The process handle we receive is a duplicate created for sending across IPC. 
      // We should clean it up.
      CloseHandle(info.source.process);

      _acknowledge();
   }
   monitor_process_state::~monitor_process_state() {
   }

   HANDLE monitor_process_state::_try_create_file_for_minidump(const std::wstring& path) {
      return CreateFileW(
         path.c_str(),
         GENERIC_READ | GENERIC_WRITE,
         FILE_SHARE_WRITE | FILE_SHARE_READ,
         nullptr,
         CREATE_ALWAYS,
         0,
         NULL
      );
   }

   void monitor_process_state::_show_message(const std::wstring& message) {
      if (this->headless) {
         if (this->console.output != NULL && this->console.output != INVALID_HANDLE_VALUE) {
            WriteFile(this->console.output, message.data(), message.size() * sizeof(wchar_t), nullptr, nullptr);
            return;
         }
      }
      MessageBoxW(
         NULL,
         message.c_str(),
         nullptr,
         MB_OK | MB_ICONERROR | MB_SYSTEMMODAL | MB_SETFOREGROUND
      );
   }

   void monitor_process_state::_acknowledge() {
      bool done = true;
      WriteFile(this->pipe.send, &done, sizeof(done), nullptr, nullptr);
   }
}