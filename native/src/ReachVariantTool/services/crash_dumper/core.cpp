#include "./core.h"
#include "./main_process_state.h"
#include "./monitor_process_state.h"
#include <windows.h>

namespace rvt::subsystems::crash_dumper {
   core::core() {
   }
   core::~core() {
   }

   void core::on_application_start(int argc, char* argv[]) {
      if (argc >= 2 && std::string_view(argv[1]) == "--crash-handler") {
         auto& state    = monitor_process_state::get_or_create();
         bool  headless = false;
         if (argc >= 3 && std::string_view(argv[2]) == "--headless") {
            headless = true;
         }
         state.main(headless);
         ExitProcess(0);
      }

      bool headless = false;
      if (argc >= 2 && std::string_view(argv[1]) == "--headless") {
         headless = true;
      }
      auto& state = main_process_state::get_or_create();
      state.setup(headless);
   }

   void core::register_new_thread() {
      auto handler = main_process_state::get_terminate_handler({});
      std::set_terminate(handler);
   }
}
