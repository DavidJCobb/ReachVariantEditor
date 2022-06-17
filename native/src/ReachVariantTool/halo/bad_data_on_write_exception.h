#pragma once
#include <stdexcept>

namespace halo {
   class bad_data_on_write_exception : public std::exception {
      public:
         bad_data_on_write_exception() {
            #if _DEBUG
               __debugbreak();
            #endif
         }
         bad_data_on_write_exception(const char* m) : std::exception(m) {
            #if _DEBUG
               __debugbreak();
            #endif
         }
   };
}
