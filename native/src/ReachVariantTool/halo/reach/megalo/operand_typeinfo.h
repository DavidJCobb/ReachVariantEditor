#pragma once
#include <cstdint>
#include "helpers/string/strcmp.h"

namespace halo::reach::megalo {
   class operand_typeinfo {
      //
      // TODO: Bring in more data from the old compiler/parser engine's OpcodeArgTypeinfo.
      //
      public:
         struct flag {
            flag() = delete;
            enum type : uint32_t {
               none = 0,
               //
               is_variable        = 0x00000001, // number, object, player, team, timer
               can_hold_variables = 0x00000002, // object, player, team
            };
         };

      public:
         uint32_t    flags         = 0;
         const char* friendly_name = nullptr; // translation key to pass to QObject::tr
         const char* internal_name = nullptr; // unique identifier for the typeinfo
         uint8_t     static_count  = 0; // e.g. 8 for player[7]

         constexpr bool can_be_static() const noexcept {
            return this->static_count > 0;
         }
         constexpr bool is_variable() const noexcept {
            return (this->flags) & flag::is_variable;
         }
         constexpr bool can_have_variables() const noexcept {
            return this->flags & flag::can_hold_variables;
         }

         // identity comparisons are broken for constexpr objects when those objects are 
         // referred to from different places.
         constexpr bool operator==(const operand_typeinfo& o) const {
            return cobb::strcmp(internal_name, o.internal_name) == 0;
         }
   };

   namespace operand_types {
      constexpr operand_typeinfo no_type = {
         .internal_name = "<none>"
      };
   }
}