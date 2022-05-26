#pragma once
#include "./reflex_enum.h"

namespace cobb {
   template<typename Enum> requires is_reflex_enum<Enum>
   class reflex_flags_from_enum {
      public:
         using enumeration     = Enum;
         using underlying_type = std::conditional_t<enumeration::explicit_underlying_type, enumeration::underlying_type, uint32_t>; // TODO: use the smallest default that can hold all flags

         static constexpr bool explicit_underlying_type = enumeration::explicit_underlying_type;
         static constexpr auto flag_count = enumeration::member_count;

         static_assert(!enumeration::has_negative_members);
         static_assert((1 << enumeration::max_underlying_value) < std::numeric_limits<underlying_type>::max(), "underlying type not large enough to store the highest flag(s)");

      protected:
         underlying_type _value = {};

      public:
         constexpr reflex_flags() {}
         template<reflex_flags... Flags> constexpr reflex_flags() {
            ((this->_value |= Flags._value), ...);
         }

         static constexpr reflex_enum from_int(underlying_type v) {
            reflex_flags out;
            out._value = 1 << v;
            return out;
         }

         template<cs Name> static constexpr bool allows = enumeration::has<Name>;

         template<cs Name> static constexpr underlying_type underlying_value_of = (1 << enumeration::underlying_value_of<Name>);
         template<cs Name> static constexpr reflex_flags value_of = from_int(enumeration::underlying_value_of<Name>);

         static constexpr reflex_flags all_set = ([]() {
            underlying_type v;
            enumeration::for_each_member([&v]<typename Current>() {
               v |= reflex_flags::underlying_value_of<Current::name>;
            });
            return v;
         })();

         template<cs Name> constexpr bool has() const {
            return (this->_value & underlying_value_of<Name>) != 0;
         }

         template<cs... Names> requires (sizeof...(Names) > 1 && (allows<Names> && ...)) constexpr reflex_flags& add() {
            ((this->_value |= underlying_value_of<Names>), ...);
            return *this;
         }
         template<cs... Names> requires (sizeof...(Names) > 1 && (allows<Names> && ...)) constexpr reflex_flags& remove() {
            ((this->_value &= ~underlying_value_of<Names>), ...);
            return *this;
         }
         template<cs... Names> requires (sizeof...(Names) > 1 && (allows<Names> && ...)) constexpr reflex_flags& modify(bool v) {
            if (v)
               return this->add(Names...);
            return this->remove(Names...);
         }

         constexpr underlying_type to_int() const {
            return _value;
         }

         constexpr bool operator==(const reflex_enum&) const = default;
         constexpr bool operator!=(const reflex_enum&) const = default;
   };

   template<typename... Parameters> using reflex_flags = reflex_flags_from_enum<reflex_enum<Parameters>>;

   template<typename T> concept is_reflex_flags = requires {
      typename T::enumeration;
      requires std::is_same_v<
         std::decay_t<T>,
         reflex_flags_from_enum<typename T::enumeration>
      >;
   };
}
