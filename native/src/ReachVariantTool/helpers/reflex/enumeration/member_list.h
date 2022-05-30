#pragma once
#include <array>
#include <tuple>
#include <type_traits>
#include "helpers/numeric_min_max.h"
#include "helpers/tuple_filter.h"
#include "helpers/tuple_foreach.h"
#include "helpers/tuple_index_of.h"
#include "helpers/unreachable.h"
#include "../member.h"
#include "../member_range.h"
#include "../nested_enum.h"
#include "./member_concepts.h"

namespace cobb::reflex::impl::enumeration {
   template<typename Underlying, typename... Members> struct member_list {
      using underlying_type = Underlying;
      using as_tuple = std::tuple<Members...>;
         
      static constexpr size_t value_count() {
         size_t count = 0;
         cobb::tuple_foreach<as_tuple>([&count]<typename Current>() {
            if constexpr (is_member<Current> || std::is_same_v<Current, member_gap>) {
               ++count;
               return;
            }
            if constexpr (is_member_range<Current>) {
               count += Current::count;
               return;
            }
            if constexpr (is_nested_enum<Current>) {
               count += Current::enumeration::member_count;
               return;
            }
            cobb::unreachable();
         });
         return count;
      };
      static constexpr size_t member_count() {
         return std::tuple_size_v<as_tuple>;
      }
      static constexpr size_t named_member_count() {
         return std::tuple_size_v<
            cobb::tuple_filter_t<
               []<typename Current>(){ return is_named_member<Current>; },
               as_tuple
            >
         >;
      }

      //
         
      template<typename Functor> static constexpr void for_each_underlying_value(Functor&& f) {
         underlying_type v = -1;
         cobb::tuple_foreach<as_tuple>([&f, &v]<typename Current>() {
            //
            // At the end of each pass, "v" should be equal to the last value for any 
            // given entry. If an entry is a single value, "v" should equal its value 
            // at the end of the pass.  If the entry is a range, "v" should equal its 
            // last value.  If the entry is a nested enum,  "v" should equal its last 
            // entry's underlying value within this superset enum.
            //
            if constexpr (std::is_same_v<Current, member_gap>) {
               ++v;
               return;
            }
            if constexpr (is_member<Current> || is_member_range<Current>) {
               if (Current::value != undefined)
                  v = Current::value;
               else
                  ++v;
               //
               if constexpr (is_member<Current>) {
                  (f)(v);
               }
               if constexpr (is_member_range<Current>) {
                  for (size_t i = 0; i < Current::count; ++i) {
                     (f)(v);
                     ++v;
                  }
                  --v;
               }
               return;
            }
            if constexpr (is_nested_enum<Current>) {
               using nested = Current::enumeration;
               constexpr auto nested_span = (nested::max_underlying_value - nested::min_underlying_value);
               constexpr auto nested_list = nested::all_underlying_values;
               //
               ++v;
               for (auto item : nested_list) {
                  (f)(v + (underlying_type)(item - nested::min_underlying_value));
               }
               v += nested_span;
               return;
            }
         });
      }

      template<cs Name> static constexpr size_t index_of() {
         return cobb::tuple_index_of_matching<
            as_tuple,
            []<typename Current>() -> bool {
               if constexpr (is_named_member<Current>)
                  return Current::name == Name;
               return false;
            }
         >();
      }

      // ---

      static constexpr size_t underlying_value_count = []() {
         size_t c = 0;
         for_each_underlying_value([&c](underlying_type) { ++c; });
         return c;
      }();

      // ---

      static constexpr std::array<const char*, named_member_count()> all_names() {
         std::array<const char*, named_member_count()> out = {};
         //
         size_t i = 0;
         cobb::tuple_foreach<as_tuple>([&i, &out]<typename Current>() {
            if constexpr (impl::enumeration::is_named_member<Current>) {
               out[i++] = Current::name.c_str();
            }
         });
         return out;
      };
      static constexpr std::array<underlying_type, member_list::underlying_value_count> all_underlying_values() {
         std::array<underlying_type, member_list::underlying_value_count> out = {};
         //
         size_t i = 0;
         for_each_underlying_value([&i, &out](underlying_type v) {
            out[i++] = v;
         });
         return out;
      }

      template<cobb::cs Name> static constexpr underlying_type underlying_value_of() {
         bool found = false;
         underlying_type v = -1;
         //
         cobb::tuple_foreach<as_tuple>([&found, &v]<typename Current>() {
            if (found) {
               return;
            }
            if constexpr (is_named_member<Current>) {
               found = Current::name == Name;
            }
            //
            // At the end of each pass, "v" should be equal to the last value for any 
            // given entry. If an entry is a single value, "v" should equal its value 
            // at the end of the pass.  If the entry is a range, "v" should equal its 
            // last value.  If the entry is a nested enum,  "v" should equal its last 
            // entry's underlying value within this superset enum.
            //
            if constexpr (std::is_same_v<Current, member_gap>) {
               ++v;
            } else if constexpr (is_member<Current> || is_member_range<Current>) {
               if (Current::value != undefined)
                  v = Current::value;
               else
                  ++v;
               if (!found) {
                  if constexpr (is_member_range<Current>) {
                     v += Current::count - 1;
                  }
               }
               return;
            } else if constexpr (is_nested_enum<Current>) {
               using nested = Current::enumeration;
               //
               if (!found) {
                  v += (nested::max_underlying_value - nested::min_underlying_value);
               }
            }
         });
         //
         if (found)
            return v;
         cobb::unreachable();
      }
         
      template<typename Subset> static constexpr size_t subset_name_length() {
         size_t length = 0;
         cobb::tuple_foreach<as_tuple>([&length]<typename Current>() {
            if constexpr (is_nested_enum<Current>)
               length = Current::name.capacity();
         });
         return length;
      }
      template<typename Subset> static constexpr cobb::cs<subset_name_length<Subset>()> subset_name() {
         constexpr auto length = subset_name_length<Subset>();
         //
         const char initial[length] = {};
         cobb::cs<length> name(initial);
         cobb::tuple_foreach<as_tuple>([&name]<typename Current>() {
            if constexpr (is_nested_enum<Current>)
               name = Current::name;
         });
         return name;
      }
   };
}