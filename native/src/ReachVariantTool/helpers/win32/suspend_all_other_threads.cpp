#include "./suspend_all_other_threads.h"
#include <utility> // std::pair
#include <windows.h>
#include <tlhelp32.h>

namespace {
   using NtGetNextThread_ptr_t = NTSTATUS(NTAPI *)(HANDLE, HANDLE, ACCESS_MASK, ULONG, ULONG, PHANDLE);
}

namespace cobb::win32 {
   extern void suspend_all_other_threads() {
      //
      // The officially recommended way to enumerate threads is by using the Toolhelp32 APIs, 
      // but they're overbroad and less efficient: they create a big array of all threads in 
      // ALL processes, not just the process we want to mess with.
      // 
      // There's an NTAPI function that's more direct. Let's see if it's available, and use 
      // it if so. (At the time of writing, it's only available from Vista onward.)
      //
      if (auto ntdll = GetModuleHandleW(L"ntdll.dll")) {
         if (auto _NtGetNextThread = (NtGetNextThread_ptr_t) GetProcAddress(ntdll, "NtGetNextThread")) {
            const HANDLE this_process   = GetCurrentProcess();
            const HANDLE this_thread    = GetCurrentThread();
            const auto   this_thread_id = GetCurrentThreadId();

            HANDLE thread_handle = NULL;
            while (true) {
               NTSTATUS result = _NtGetNextThread(
                  this_process,          // process whose threads we want to access
                  thread_handle,         // previous handle (think of Lua `next`)
                  THREAD_SUSPEND_RESUME, // permissions we want to be able to take; threads that we can't get these perms for are skipped
                  0,                     // desired attributes of handle to create
                  0,                     // reserved; keep at zero
                  &thread_handle         // out
               );
               if (thread_handle == NULL || thread_handle == INVALID_HANDLE_VALUE)
                  break;

               if (GetThreadId(thread_handle) != this_thread_id) {
                  SuspendThread(thread_handle);
               }
               CloseHandle(thread_handle);
            }

            // done
            CloseHandle(ntdll);
            return;
         }
         // done
         CloseHandle(ntdll);
      }
      //
      // Fall back to the Toolhelp32 API.
      //
      HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, NULL);
      if (snapshot == INVALID_HANDLE_VALUE)
         return;
      THREADENTRY32 thread_entry;
      thread_entry.dwSize = sizeof(thread_entry);
      if (Thread32First(snapshot, &thread_entry)) {
         //
         // We'll loop over every thread (across basically every process in the system, so we 
         // do need to specifically check that any given thread belongs to *our process*).
         //
         const auto this_thread  = GetCurrentThreadId();
         const auto this_process = GetCurrentProcessId();
         do {
            if (thread_entry.dwSize >= FIELD_OFFSET(THREADENTRY32, th32OwnerProcessID) + sizeof(thread_entry.th32OwnerProcessID)) {
               if (
                  thread_entry.th32OwnerProcessID == this_process &&
                  thread_entry.th32ThreadID != this_thread
               ) {
                  const auto thread_handle = OpenThread(THREAD_SUSPEND_RESUME, FALSE, thread_entry.th32ThreadID);
                  if (thread_handle != NULL && thread_handle != INVALID_HANDLE_VALUE) {
                     SuspendThread(thread_handle);
                     CloseHandle(thread_handle);
                  }
               }
            }
            thread_entry.dwSize = sizeof(thread_entry);
         } while (Thread32Next(snapshot, &thread_entry));
      }
      CloseHandle(snapshot);
   }
}