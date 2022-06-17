#pragma once

namespace halo::util {
   template<typename Writer, typename T> concept has_write_method = requires (const T& x, Writer& stream) {
      { x.write(stream) };
   };
}