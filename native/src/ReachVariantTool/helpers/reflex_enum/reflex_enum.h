#pragma once
#include "./member_list.h"
#include "./cs.h"
#include "./member.h"

namespace cobb {
   namespace impl::reflex_enum {
      template<typename First, typename... Members> struct extract_typeinfo {
         static constexpr bool explicit_underlying_type = std::is_integral_v<First> && std::is_arithmetic_v<First>;

         static constexpr size_t member_count = sizeof...(Members) + (explicit_underlying_type ? 0 : 1);
         using members_tuple = std::conditional_t<
            explicit_underlying_type,
            std::tuple<Members...>,
            std::tuple<First, Members...>
         >;
         using members = member_list_from_tuple<members_tuple>::type;

         using underlying_type = std::conditional_t<
            explicit_underlying_type,
            First,
            int
         >;

         static constexpr bool members_are_valid = (
            (is_member_type<Members> && ...) && (explicit_underlying_type || is_member_type<First>)
         && members::all_member_names_unique
         && members::template all_explicit_member_values_representable<underlying_type>
         );
      };
   }

   template<typename... Parameters> requires impl::reflex_enum::extract_typeinfo<Parameters...>::members_are_valid
   struct reflex_enum {
      protected:
         using _extractor = impl::reflex_enum::extract_typeinfo<Parameters...>;

      public:
         using underlying_type = _extractor::underlying_type;
         using members = _extractor::members;
         static constexpr size_t member_count = _extractor::member_count;
         static constexpr size_t missing_type = std::numeric_limits<size_t>::max();

         template<cs Name> static constexpr size_t index_of = members::index_of_name<Name> + 0; // addition is a bandaid to fix MSVC 2022 C2760 as of 5/25/22
         template<cs Name> static constexpr bool has = (index_of<Name> != missing_type);

      protected:
         underlying_type _value = {};

      protected:
         static constexpr auto _all_names = members::all_names();
         static constexpr auto _underlying_values = members::template all_underlying_values<underlying_type>();

         template<typename Functor> static constexpr void _for_each_member_type(Functor&& f) {
            members::for_each(f);
         }

      public:
         constexpr reflex_enum() {}
         template<cs Name> requires has<Name> static constexpr reflex_enum make() {
            return value_of<Name>;
         }

         static constexpr reflex_enum from_int(underlying_type v) {
            reflex_enum out;
            out._value = v;
            return out;
         }

         template<typename Functor> static constexpr void for_each_member(Functor&& f) {
            members::for_each(f);
         }

         template<cs Name> requires (has<Name>) static constexpr underlying_type underlying_value_of = _underlying_values[index_of<Name>];
         template<cs Name> requires (has<Name>) static constexpr reflex_enum value_of = ([]() {
            reflex_enum out;
            out._value = underlying_value_of<Name>;
            return out;
         })();

         template<cs Name> requires has<Name> constexpr reflex_enum& assign() {
            this->_value = underlying_value_of<Name>;
         }

         constexpr underlying_type to_int() const {
            return _value;
         }
         constexpr const char* to_string() const {
            size_t i;
            for (i = 0; i < _underlying_values.size(); ++i)
               if (_underlying_values[i] == this->_value)
                  break;
            if (i >= _underlying_values.size())
               return nullptr;
            //
            return _all_names[i];
         }

         constexpr bool operator==(const reflex_enum&) const = default;
         constexpr bool operator!=(const reflex_enum&) const = default;
         //
         template<cs Name> constexpr bool is() const {
            if constexpr (has<Name>)
               return this->_value == underlying_value_of<Name>;
            else
               return false;
         }
   };
}