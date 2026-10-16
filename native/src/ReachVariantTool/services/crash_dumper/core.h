#pragma once
#include "helpers/singleton_ex.h"

namespace rvt::subsystems::crash_dumper {
   class core : public cobb::singleton_ex<core> {
      protected:
         core();
         ~core();

      public:
         // Call this from `main`.
         // If we're running in the monitor process, this function will not return.
         void on_application_start(int argc, char* argv[]);

         void register_new_thread();
   };
}