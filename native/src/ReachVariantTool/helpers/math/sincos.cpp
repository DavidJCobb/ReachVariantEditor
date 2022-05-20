#include "sincos.h"
#include "./cosine.h"
#include "./sine.h"
#include "./rotation/unit_conversion.h"

namespace cobb::tests {
   //
   // Validate cobb::sincos to ensure it produces consistent results with cobb::sine and cobb::cosine:
   //
   namespace sincos {
      // Test a single degree value:
      template<int Degrees> constexpr bool is_consistent = ([]() {
         constexpr double radians = (double)Degrees * degrees_to_radians_mult;
         //
         constexpr double s = ::cobb::sine(radians);
         constexpr double c = ::cobb::cosine(radians);
         double sc_s = {};
         double sc_c = {};
         ::cobb::sincos(radians, sc_s, sc_c);
         return (sc_s - s) == 0 && (sc_c - c) == 0;
      })();

      // Test multiple degree values:
      template<int... Degrees> constexpr bool all_consistent = (is_consistent<Degrees> && ...);

      // Helper template to test an entire range from 0 to N:
      template<typename S = std::make_index_sequence<5>> struct range_consistent;
      template<size_t... Values> struct range_consistent<std::index_sequence<Values...>> {
         static constexpr bool result = all_consistent<Values...>;
      };

      // Static assertion to run the test:
      static_assert(range_consistent<std::make_index_sequence<360>>::result);
   }
}