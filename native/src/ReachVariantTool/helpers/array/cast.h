#pragma once
#include <array>
#include <type_traits>

namespace cobb::array {
   template<typename DstType, typename SrcType, size_t Size> requires std::is_convertible_v<SrcType, DstType>
   consteval std::array<DstType, Size> cast(const std::array<SrcType, Size>& src) {
      std::array<DstType, Size> dst = {};
      for (size_t i = 0; i < Size; ++i)
         dst[i] = static_cast<DstType>(src[i]);
      return dst;
   }
}