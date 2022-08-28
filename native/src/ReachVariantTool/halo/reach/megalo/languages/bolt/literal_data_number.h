#pragma once
#include <cstdint>

namespace halo::reach::megalo::bolt {
   class literal_data_number {
      public:
         enum format_type {
            none,    // uninitialized
            decimal, // double
            integer, // int32_t
         };

      public:
         literal_data_number() {}
         literal_data_number(const literal_data_number& n);
         literal_data_number& operator=(const literal_data_number& n);

      public:
         format_type format = format_type::none;
         union {
            double  decimal;
            int32_t integer = 0; // NOTE: Megalo uses 16-bit values.
         } value;

         bool is_valid() const;
   };
}