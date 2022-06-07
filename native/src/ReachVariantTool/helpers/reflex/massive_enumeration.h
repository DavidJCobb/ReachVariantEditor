#pragma once
#include <array>
#include <limits>
#include <string>
#include <type_traits>
#include "helpers/string/strcmp.h"
#include "helpers/cs.h"

namespace cobb::reflex {
   struct massive_enumeration_params {
      int32_t start = 0;
   };

   // Reflex enumerations with large numbers of items (366 tested) cause IntelliSense to crash, and 
   // cause the compiler to consume over 30 GB of memory if you don't kill the process. This class 
   // therefore exists as a simpler, stripped-down alternative for massive enumerations.
   //
   // The following functionality is omitted:
   // 
   //  - Per-value explicit underlying values
   //  - Per-value metadata structs
   //  - Range members
   //  - Nested enums
   //  - All functionality that requires that values be specified as types
   //     - member_type<Name>
   //
   template<massive_enumeration_params Params, const auto& Names> class massive_enumeration {
      protected:
         static constexpr auto _index_of_none = std::numeric_limits<size_t>::max();

      public:
         static constexpr auto params    = Params;
         static constexpr auto all_names = Names;

         using underlying_type = int32_t;

         static constexpr bool explicit_underlying_type = false;

         static constexpr size_t value_count = all_names.size();

         static constexpr underlying_type min_underlying_value = params.start;
         static constexpr underlying_type max_underlying_value = value_count + params.start;

         static constexpr underlying_type underlying_value_range = max_underlying_value - min_underlying_value + 1;

      protected:
         template<cs Name> static constexpr size_t index_of = [](){
            constexpr const char* const desired = Name.c_str();
            for (size_t i = 0; i < all_names.size(); ++i)
               if (cobb::strcmp(desired, all_names[i]) == 0)
                  return i;
            return std::numeric_limits<size_t>::max();
         }();

      protected:
         underlying_type _value = 0;

      public:
         template<cs Name> static constexpr bool has = (index_of<Name> != _index_of_none);

      public:
         constexpr massive_enumeration() {}
         explicit constexpr massive_enumeration(underlying_type v) { return from_int(v); }

         template<typename V> requires std::is_convertible_v<V, underlying_type> constexpr static massive_enumeration from_int(V v) {
            enumeration out;
            out._value = v;
            return out;
         }

         constexpr underlying_type to_int() const {
            return this->_value;
         }
         constexpr const char* to_c_str() const {
            return all_names[this->_value - params.start];
         }
         std::string to_string() const {
            auto p = to_c_str();
            return p ? p : std::string{};
         }

         explicit operator underlying_type() const { return this->_value; }

         constexpr bool operator==(const massive_enumeration&) const = default;
         constexpr bool operator!=(const massive_enumeration&) const = default;

      public:
         static constexpr massive_enumeration min_value() { return from_int(min_underlying_value); }
         static constexpr massive_enumeration max_value() { return from_int(max_underlying_value); }
         
         template<cs Name> requires has<Name> static constexpr underlying_type underlying_value_of = [](){
            return index_of<Name> + params.start;
         }();

         template<cs Name> requires has<Name> static constexpr massive_enumeration value_of = [](){
            return from_int(underlying_value_of<Name>);
         }();
   };

}
