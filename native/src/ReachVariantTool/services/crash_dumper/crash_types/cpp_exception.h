#pragma once
#include <stdexcept>
#include <string>
#include "helpers/win32/forward_declare_handles.h"

namespace rvt::subsystems::crash_dumper::crash_types {
   struct cpp_exception {
      std::wstring what;

      static void send(HANDLE stream, const std::exception&);
      bool read(HANDLE stream);
   };
}