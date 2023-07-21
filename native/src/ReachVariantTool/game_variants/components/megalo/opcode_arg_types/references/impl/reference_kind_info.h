#pragma once
#include <array>
#include <optional>
#include "../type.h"
#include "./enum_with_explicit_end.h"

namespace Megalo::references {
   struct kind_info {
      reference_type top_level_type;
      std::optional<reference_type> member_type;
      const char* property_name = nullptr;
   };

   template<references::impl::enum_with_explicit_end ReferenceKindEnum>
   struct kind_info_set {
      static constexpr const size_t count = (size_t)ReferenceKindEnum::__enum_end;

      std::array<kind_info, count> info;

      template<class Self>
      constexpr auto&& operator[](this Self&& self, ReferenceKindEnum e) {
         return self.info[(size_t)e];
      }

      template<class Self>
      constexpr auto&& operator[](this Self&& self, size_t i) {
         return self.info[i];
      }

      constexpr const size_t size() const noexcept { return info.size(); }
   };

   // Specialize this.
   template<typename ReferenceKindEnum>
   struct kind_info_set_for {
      static constexpr const kind_info* first = nullptr;
      static constexpr const size_t     count = 0;
   };
}