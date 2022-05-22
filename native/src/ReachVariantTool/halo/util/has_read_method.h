#pragma once

namespace halo::util {
   template<typename Reader, typename T> concept has_read_method = requires (T& x, Reader& stream) {
      { x.read(stream) };
   };
}