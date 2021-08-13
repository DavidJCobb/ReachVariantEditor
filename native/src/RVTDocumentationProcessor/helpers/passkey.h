#pragma once

namespace cobb {
   template<typename A, typename B> class passkey {
      friend A;
      friend B;
      private:
         constexpr passkey() {}
         constexpr passkey(const passkey&) {}
   };
}
