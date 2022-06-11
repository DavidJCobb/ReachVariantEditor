#pragma once
#include <stdexcept>
#include <QString>

namespace halo {
   class load_process_fatal_error : public std::exception {
      public:
         load_process_fatal_error() : std::exception() {}
   };
}