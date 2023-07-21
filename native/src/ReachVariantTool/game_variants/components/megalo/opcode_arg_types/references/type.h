#pragma once

namespace Megalo {
   enum class reference_type {
      // Same order as the file format's own enums, for simplicity's sake.
      number,
      player,
      object,
      team,
      timer,
   };

   template<reference_type RefType>
   constexpr const bool reference_type_is_handle_type = []() -> bool {
      switch (RefType) {
         case reference_type::object:
         case reference_type::player:
         case reference_type::team:
            return true;
      }
      return false;
   }();
}