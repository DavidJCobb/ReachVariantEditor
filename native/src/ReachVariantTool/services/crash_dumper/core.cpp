#include "./core.h"
#include "./main_process_state.h"

namespace rvt::subsystems::crash_dumper {
   core::core() {
   }
   core::~core() {
   }

   void core::register_new_thread() {
      auto handler = main_process_state::get_terminate_handler({});
      std::set_terminate(handler);
   }
}
