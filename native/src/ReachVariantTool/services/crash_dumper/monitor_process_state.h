#pragma once
#include <cstdint>
#include <string>
#include "helpers/singleton_ex.h"
#include "helpers/win32/forward_declare_handles.h"

namespace rvt::subsystems::crash_dumper {
   class monitor_process_state : public cobb::singleton_ex<monitor_process_state> {
      protected:
         monitor_process_state();
         ~monitor_process_state();

      public:
         void main(bool headless);

      protected:
         struct {
            HANDLE send    = NULL;
            HANDLE receive = NULL;
         } pipe;
         struct {
            HANDLE input  = NULL;
            HANDLE output = NULL;
         } console;
         bool headless = false;

      private:
         HANDLE _try_create_file_for_minidump(const std::wstring& path);

         void _show_message(const std::wstring&);

         void _acknowledge();
   };
}