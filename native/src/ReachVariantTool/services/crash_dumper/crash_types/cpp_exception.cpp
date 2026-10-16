#include "./cpp_exception.h"
#include "../ipc.h"

namespace rvt::subsystems::crash_dumper::crash_types {
   /*static*/ void cpp_exception::send(HANDLE stream, const std::exception& ex) {
      ipc::send_length_prefixed_string(stream, ex.what());
   }
   bool cpp_exception::read(HANDLE stream) {
      return ipc::read_length_prefixed_string(stream, this->what);
   }
}