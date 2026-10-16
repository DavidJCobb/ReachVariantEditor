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
   #pragma region Strings for initially notifying the user
      constexpr const std::wstring_view intro_generic_terminate =
         L"ReachVariantTool has encountered an irrecoverable error (basically, a crash). An internal "
         L"function called `std::terminate` was called; there's no available information as to "
         L"why."
      ;

      constexpr const std::wstring_view intro_generic_throw =
         L"ReachVariantTool has encountered an irrecoverable error (basically, a crash). An error was "
         L"thrown and not caught, but the error wasn't a normal std::exception, so no further "
         L"information is available."
      ;

      constexpr const std::wstring_view intro_cpp_exception_with_text =
         L"ReachVariantTool has encountered an irrecoverable error (basically, a crash). An exception "
         L"was thrown but not caught. The exception had the following text:"
      ;

      constexpr const std::wstring_view intro_cpp_exception_without_text =
         L"ReachVariantTool has encountered an irrecoverable error (basically, a crash). An exception "
         L"was thrown but not caught. The exception didn't have any text information."
      ;
   
      constexpr const std::wstring_view intro_structured_exception =
         L"ReachVariantTool has encountered an irrecoverable error (basically, a crash)."
      ;
   
      constexpr const std::wstring_view intro_addendum_copy_and_paste =
         L"(You can press Ctrl + C on this dialog to copy its text. Please do so, so you can "
         L"send as much information to ReachVariantTool's developer as possible.)"
      ;

      constexpr const std::wstring_view ask_about_minidumps =
         L"ReachVariantTool will attempt to save error information into \"minidump\" files, which you "
         L"can send to the developer to help them investigate the crash. ReachVariantTool will save a "
         L"small minidump. If you want, it can also save a full minidump containing everything "
         L"ReachVariantTool has in memory, but that could potentially take up multiple gigabytes.\n"
         L"\n"
         L"Do you want to save both minidumps?"
      ;
   #pragma endregion

   #pragma region Strings for telling the user how the minidump went
      constexpr const std::wstring_view minidump_success_small =
         L"ReachVariantTool has crashed. The \"small\" minidump was saved to this file:"
      ;

      constexpr const std::wstring_view minidump_success_large =
         L"The \"large\" minidump was saved to this file:"
      ;

      constexpr const std::wstring_view minidump_success_large_only =
         L"ReachVariantTool has crashed. It failed to save a \"small\" minidump, but strangely, it seems to have "
         L"saved the \"large\" minidump just fine. You can find the \"large\" minidump here:\n\n"
      ;

      constexpr const std::wstring_view minidump_large_failed_but_small_succeeded =
         L"ReachVariantTool failed to save the \"large\" minidump."
      ;

      constexpr const std::wstring_view minidump_how_to_copy_one_path =
         L"If you press Ctrl + C while this dialog is open, you should be able to copy its text, to get the file path."
      ;
      
      constexpr const std::wstring_view minidump_how_to_copy_all_paths =
         L"If you press Ctrl + C while this dialog is open, you should be able to copy its text, to get the file paths."
      ;

      constexpr const std::wstring_view minidump_dont_clobber_previous_error_info =
         L"If ReachVariantTool asked you to copy any other error information, then make sure you save that somewhere first, " \
         L"so you can send everything to ReachVariantTool's developer."
      ;

      constexpr const std::wstring_view minidump_please_save_copies_of_your_files =
         L"If you were working with any of your own files at the time of the crash (e.g. a mod you were making), " \
         L"please also save a copy of those files somewhere, in case ReachVariantTool's developer asks to look at those " \
         L"files' exact data (i.e. at the time of the crash) later on."
      ;

      constexpr const std::wstring_view minidump_all_failed =
         L"ReachVariantTool has crashed. Unfortunately, it was unable to output any \"minidump\" files that could "
         L"be used to debug the crash.\n\n"
         L"Please contact ReachVariantTool's developer and tell them about this crash. Try to remember as much as "
         L"you can about what you were doing when the crash happened."
         L" " L"If ReachVariantTool asked you to copy any other error information, please send that to ReachVariantTool's developer."
      ;
   #pragma endregion
}

namespace rvt::subsystems::crash_dumper {
   monitor_process_state::monitor_process_state() {
      this->pipe.send    = GetStdHandle(STD_OUTPUT_HANDLE);
      this->pipe.receive = GetStdHandle(STD_INPUT_HANDLE);
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
         // terminal: do a basic handshake with the parent process.
         //
         HANDLE parent_process = NULL;
         DWORD  parent_proc_id = 0;
         if (
            !ipc::read_value(this->pipe.receive, parent_process) ||
            !ipc::read_value(this->pipe.receive, parent_proc_id)
         ) {
            std::wcout << L"Crash-dumping process failed the handshake. (No IPC?)\n";
            std::wcout << L"(Did you manually run RVT with the --crash-handler switch? Please don't.)\n";
            ExitProcess(1);
            return;
         }
         if (
            parent_process == NULL ||
            parent_process == INVALID_HANDLE_VALUE
         ) {
            std::wcout << L"Crash-dumping process failed the handshake. (Bad parent process handle.)\n";
            std::wcout << L"(Did you manually run RVT with the --crash-handler switch? Please don't.)\n";
            ExitProcess(1);
            return;
         }
         DWORD id = GetProcessId(parent_process);
         CloseHandle(parent_process);
         if (id != parent_proc_id) {
            std::wcout << L"Crash-dumping process failed the handshake. (Process ID didn't match.)\n";
            std::wcout << L"(Did you manually run RVT with the --crash-handler switch? Please don't.)\n";
            ExitProcess(1);
            return;
         }
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

      std::wstring message;
      #pragma region Build string to notify the user, and grab EXCEPTION_POINTERS if present
         if (auto* casted = std::get_if<crash_types::cpp_exception>(&info.data)) {
            if (casted->what.empty()) {
               message += intro_cpp_exception_without_text;
            } else {
               message += intro_cpp_exception_with_text;
               message += L"\n\n";
               message += std::move(casted->what);
            }
            message += L"\n\n";
            message += intro_addendum_copy_and_paste;
            message += L"\n\n";
            message += ask_about_minidumps;
         } else if (auto* casted = std::get_if<crash_types::cpp_generic_terminate>(&info.data)) {
            message.reserve(intro_generic_terminate.size() + 2 + ask_about_minidumps.size());
            message += intro_generic_terminate;
            message += L"\n\n";
            message += intro_addendum_copy_and_paste;
            message += L"\n\n";
            message += ask_about_minidumps;
         } else if (auto* casted = std::get_if<crash_types::cpp_generic_throw>(&info.data)) {
            message.reserve(intro_generic_throw.size() + 2 + ask_about_minidumps.size());
            message += intro_generic_throw;
            message += L"\n\n";
            message += intro_addendum_copy_and_paste;
            message += L"\n\n";
            message += ask_about_minidumps;
         } else if (auto* casted = std::get_if<crash_types::structured_exception>(&info.data)) {
            message += intro_structured_exception;
            message += L"\n\n";
            message += ask_about_minidumps;

            if (casted->context.has_value()) {
               ep.ContextRecord = &casted->context.value();
            }
            if (!casted->records.empty()) {
               ep.ExceptionRecord = &casted->records[0];
            }
         }
      #pragma endregion

      auto answer = MessageBoxW(
         NULL,
         message.c_str(),
         nullptr,
         MB_YESNO | MB_ICONERROR | MB_SYSTEMMODAL | MB_SETFOREGROUND
      );
      const bool user_wants_large = answer == IDYES;

      struct {
         std::wstring small;
         std::wstring large;
      } minidump_paths;
      struct {
         bool small = false;
         bool large = false;
      } minidumps_saved;

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

         auto p_exception_info = &exception_info;
         if (!ep.ContextRecord || !ep.ExceptionRecord)
            p_exception_info = nullptr;

         SYSTEMTIME local_time;
         GetLocalTime(&local_time);

         std::wstring path = get_or_create_minidump_folder();

         minidump_paths.small = path + minidump_filename(local_time, process_id, info.source.thread_id, false);
         if (user_wants_large)
            minidump_paths.large = path + minidump_filename(local_time, process_id, info.source.thread_id, true);

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

         HANDLE dump_file = _try_create_file_for_minidump(minidump_paths.small);
         if (dump_file != INVALID_HANDLE_VALUE) {
            minidumps_saved.small = _try_minidump(dump_file, small_minidump_flags);
            if (!minidumps_saved.small) {
               #if _DEBUG
                  minidumps_saved.small = false; // put a breakpoint here
               #endif
            }
            CloseHandle(dump_file);
         }
         if (user_wants_large) {
            dump_file = _try_create_file_for_minidump(minidump_paths.large);
            if (dump_file != INVALID_HANDLE_VALUE) {
               minidumps_saved.large = _try_minidump(dump_file, large_minidump_flags);
               if (!minidumps_saved.large) {
                  #if _DEBUG
                     minidumps_saved.large = false; // put a breakpoint here
                  #endif
               }
               CloseHandle(dump_file);
            }
         }
      }
      
      message.clear();
      #pragma region Build string to report minidump results
         if (minidumps_saved.small) {
            message  = minidump_success_small;
            message += L"\n\n";
            message += minidump_paths.small;
            message += L"\n\n";
            if (minidumps_saved.large) {
               message += minidump_success_large;
               message += L"\n\n";
               message += minidump_paths.large;
               message += L"\n\n";
            } else if (user_wants_large) {
               message += minidump_large_failed_but_small_succeeded;
               message += L"\n\n";
            }
         } else if (minidumps_saved.large) {
            message = minidump_success_large_only;
            message += L"\n\n";
            message += minidump_paths.large;
            message += L"\n\n";
         } else {
            message = minidump_all_failed;
         }
         if (minidumps_saved.small || minidumps_saved.large) {
            if (minidumps_saved.small && minidumps_saved.large) {
               message += minidump_how_to_copy_all_paths;
            } else {
               message += minidump_how_to_copy_one_path;
            }
            message += L" ";
            message += minidump_dont_clobber_previous_error_info;
         }
         message += L" ";
         message += minidump_please_save_copies_of_your_files;
      #pragma endregion
      MessageBoxW(
         NULL,
         message.c_str(),
         nullptr,
         MB_OK | MB_ICONERROR | MB_SYSTEMMODAL | MB_SETFOREGROUND
      );

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

   void monitor_process_state::_acknowledge() {
      bool done = true;
      WriteFile(this->pipe.send, &done, sizeof(done), nullptr, nullptr);
   }
}