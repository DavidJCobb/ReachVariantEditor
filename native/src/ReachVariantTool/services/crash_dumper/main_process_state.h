#pragma once
#include <mutex>
#include "helpers/singleton_ex.h"
#include "helpers/win32/forward_declare_handles.h"
#include "helpers/win32/WINAPI.define.h"
struct _EXCEPTION_POINTERS; // Win32
namespace rvt::subsystems::crash_dumper {
   class core;
}

namespace rvt::subsystems::crash_dumper {
   class main_process_state : public cobb::singleton_ex<main_process_state> {
      public:
         class core_passkey {
            friend core;
            friend main_process_state;
            private:
               constexpr core_passkey() {}
               constexpr ~core_passkey() {}
         };

         static std::terminate_handler get_terminate_handler(core_passkey) {
            return &_terminate_handler;
         }

      protected:
         main_process_state();
         ~main_process_state();

         [[noreturn]] static void _terminate_handler();
         static long WINAPI _unhandled_exception_filter(_EXCEPTION_POINTERS*);

      protected:
         std::mutex mutex;
         HANDLE child_process = NULL;
         struct {
            HANDLE send    = NULL;
            HANDLE receive = NULL;
         } pipe;

      private:
         void _spawn_child_process();

         static main_process_state& _get_and_lock();

         void _wait_for_acknowledge();
   };
}

#undef WINAPI