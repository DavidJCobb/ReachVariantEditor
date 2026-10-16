#include "./get_parent_process_id.h"
#include <windows.h>
#include <tlhelp32.h>

namespace cobb::win32 {
   extern std::optional<uint32_t> get_parent_process_id() {
      std::optional<uint32_t> result;
      HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

      PROCESSENTRY32W pe = { 0 };
      pe.dwSize = sizeof(PROCESSENTRY32W);

      const DWORD our_process_id = GetCurrentProcessId();
      if (Process32FirstW(snapshot, &pe)) {
         do {
            if (pe.th32ProcessID == our_process_id) {
               result = pe.th32ParentProcessID;
               break;
            }
         } while (Process32NextW(snapshot, &pe));
      }

      CloseHandle(snapshot);
      return result;
   }
}