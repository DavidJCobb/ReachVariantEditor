#include "./main_process_state.h"
#include <windows.h>
#include "helpers/win32/suspend_all_other_threads.h"
#include "./crash_info.h"
#include "./ipc.h"

constexpr const SECURITY_ATTRIBUTES heritable_handle_attributes = {
   .nLength              = sizeof(SECURITY_ATTRIBUTES),
   .lpSecurityDescriptor = nullptr,
   .bInheritHandle       = TRUE,
};
constexpr const SECURITY_ATTRIBUTES non_heritable_handle_attributes = {
   .nLength              = sizeof(SECURITY_ATTRIBUTES),
   .lpSecurityDescriptor = nullptr,
   .bInheritHandle       = FALSE,
};

namespace rvt::subsystems::crash_dumper {
   main_process_state::main_process_state() {
      SetUnhandledExceptionFilter(&_unhandled_exception_filter);
      std::set_terminate(&_terminate_handler);
   }
   main_process_state::~main_process_state() {
   }

   void main_process_state::setup(bool headless) {
      if (this->child_process != NULL)
         return;
      this->_spawn_child_process(headless);
   }

   void main_process_state::_spawn_child_process(bool headless) {
      struct {
         struct {
            HANDLE read  = NULL; // handle the child process uses to read from stdin
            HANDLE write = NULL; // handle we use to send data to the child process's stdin
         } std_in;
         struct {
            HANDLE read  = NULL; // handle we use to read data from the child process's stdout
            HANDLE write = NULL; // handle the child process uses to write to stdout
         } std_out;
      } child_handles;

      auto security_attr = heritable_handle_attributes;

      if (!CreatePipe(&child_handles.std_out.read, &child_handles.std_out.write, &security_attr, 0)) {
         return;
      }
      SetHandleInformation(child_handles.std_out.read, HANDLE_FLAG_INHERIT, 0); // suppress inheritance of the handle we use to receive data

      if (!CreatePipe(&child_handles.std_in.read, &child_handles.std_in.write, &security_attr, 0)) {
         return;
      }
      SetHandleInformation(child_handles.std_in.write, HANDLE_FLAG_INHERIT, 0); // suppress inheritance of the handle we use to send data

      //
      // Create the child process.
      //

      std::wstring command_line = L"ReachVariantTool.exe --crash-handler"; // cannot be const; CreateProcessW may modify it
      if (headless) {
         command_line += L" --headless";
      }

      PROCESS_INFORMATION process_info;
      STARTUPINFOW        startup_info;
      memset(&process_info, 0, sizeof(process_info));
      memset(&startup_info, 0, sizeof(startup_info));
      startup_info.cb         = sizeof(startup_info);
      startup_info.hStdError  = child_handles.std_out.write;
      startup_info.hStdOutput = child_handles.std_out.write;
      startup_info.hStdInput  = child_handles.std_in.read;
      startup_info.dwFlags   |= STARTF_USESTDHANDLES;

      bool success = CreateProcessW(
         NULL,                // application name
         command_line.data(), // command line
         NULL,                // process security attributes
         NULL,                // primary thread security attributes
         TRUE,                // inherit handles
         0,                   // creation flags
         NULL,                // environment variables to use (NULL = same as parent)
         NULL,                // current working directory (NULL = same as parent)
         &startup_info,       // STARTUPINFO pointer
         &process_info        // out: result (child process ID and its main thread ID)
      );

      // Close the handles that are only used by the child process, so that that 
      // process has sole ownership of them (and sole influence on their lifetimes).
      CloseHandle(child_handles.std_out.write);
      CloseHandle(child_handles.std_in.read);

      if (success) {
         this->child_process = process_info.hProcess;
         this->pipe.send     = child_handles.std_in.write;
         this->pipe.receive  = child_handles.std_out.read;
         //
         // Send our output console handle, for use if we're running headlessly.
         //
         ipc::send_value(this->pipe.send, GetStdHandle(STD_OUTPUT_HANDLE));
      } else {
         #if _DEBUG
            __debugbreak();
         #endif
      }
   }

   [[noreturn]] /*static*/ void main_process_state::_terminate_handler() {
      auto& self  = _get_and_lock();
      if (!self.child_process) {
         std::abort();
      }

      //
      // Some implementations (e.g. MSVC) will allocate exceptions on the stack when 
      // thrown, and transfer them to the heap (incurring a heap allocation) if you 
      // call this function. It is thus important that we call this function *before* 
      // we suspend all other threads. If we suspend other threads first, and one of 
      // those threads was in the middle of an allocation or free, then that thread 
      // will have the heap locked, and so we'll deadlock.
      //
      auto e_ptr = std::current_exception();

      cobb::win32::suspend_all_other_threads();

      if (!e_ptr) {
         crash_info::send<crash_types::cpp_generic_terminate>(self.pipe.send, self.child_process);
      } else {
         try {
            std::rethrow_exception(e_ptr);
         } catch (const std::exception& e) {
            crash_info::send<crash_types::cpp_exception>(self.pipe.send, self.child_process, e);
         } catch (...) {
            crash_info::send<crash_types::cpp_generic_throw>(self.pipe.send, self.child_process);
         }
      }

      self._wait_for_acknowledge();

      // Allow process to terminate.
      _set_abort_behavior(0, _WRITE_ABORT_MSG);
      std::abort();
   }
   /*static*/ long WINAPI main_process_state::_unhandled_exception_filter(_EXCEPTION_POINTERS* ep) {
      auto& self = _get_and_lock();
      if (!self.child_process) {
         return EXCEPTION_EXECUTE_HANDLER;
      }

      cobb::win32::suspend_all_other_threads();

      crash_info::send<crash_types::structured_exception>(self.pipe.send, self.child_process, *ep);

      self._wait_for_acknowledge();

      // Allow process to terminate.
      return EXCEPTION_EXECUTE_HANDLER;
   }

   /*static*/ main_process_state& main_process_state::_get_and_lock() {
      auto& self = main_process_state::get();

      // We deliberately never release this. We specifically want to ensure that if multiple 
      // threads throw SEH or other exceptions, the later-failing threads' handlers don't run 
      // to completion (which might kill the program before the first failing thread's handler 
      // finishes reporting the crash).
      self.mutex.lock();

      return self;
   }

   void main_process_state::_wait_for_acknowledge() {
      bool handled = false;
      ipc::read_value(this->pipe.receive, handled);
   }
}