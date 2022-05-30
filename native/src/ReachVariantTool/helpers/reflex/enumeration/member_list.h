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

      struct value_metadata {
         underlying_type value;
         size_t type_index;
         size_t type_sub = 0;
      };
         
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
      static constexpr size_t member_count() { // including gaps
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

      static constexpr auto underlying_value_metadata() {
         std::array<value_metadata, value_count()> out = {};
         //
         underlying_type v = -1;
         size_t type_index  = -1;
         size_t value_index = -1;
         cobb::tuple_foreach<as_tuple>([&v, &type_index, &value_index, &out]<typename Current>() {
            ++type_index;
            if constexpr (std::is_same_v<Current, member_gap>) {
               ++v;
               return;
            }
            //
            if constexpr (is_member<Current> || is_member_range<Current>) {
               if (Current::value != undefined)
                  v = Current::value;
               else
                  ++v;
               //
               if constexpr (is_member<Current>) {
                  out[++value_index] = {
                     .value      = v,
                     .type_index = type_index,
                  };
               } else if constexpr (is_member_range<Current>) {
                  for (size_t i = 0; i < Current::count; ++i) {
                     out[++value_index] = {
                        .value      = v,
                        .type_index = type_index,
                        .type_sub   = i,
                     };
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
               for (size_t i = 0; i < nested_list.size(); ++i) {
                  auto item = nested_list[i];
                  //
                  out[++value_index] = {
                     .value      = (v + (underlying_type)(item - nested::min_underlying_value)),
                     .type_index = type_index,
                     .type_sub   = i,
                  };
               }
               v += nested_span;
               return;
            }
         });
         //
         return out;
      };

      //

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
      template<typename Subset> static constexpr size_t index_of_subset() {
         return cobb::tuple_index_of_matching<
            as_tuple,
            []<typename Current>() {
               if constexpr (is_nested_enum<Current>)
                  return std::is_same_v<Subset, typename Current::enumeration>;
               return false;
            }
         >();
      };

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
      static constexpr auto all_underlying_values() {
         constexpr auto meta = underlying_value_metadata();
         //
         std::array<underlying_type, meta.size()> out = {};
         for (size_t i = 0; i < meta.size(); ++i)
            out[i] = meta[i].value;
         return out;
      }

      template<cobb::cs Name> static constexpr underlying_type underlying_value_of() {
         constexpr auto index = index_of<Name>();
         for (const auto& item : underlying_value_metadata())
            if (item.type_index == index)
               return item.value;
         cobb::unreachable();
      }
      
      template<typename Subset> static constexpr size_t subset_name_length() {
         return std::tuple_element_t<index_of_subset<Subset>(), as_tuple>::name.capacity();

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

      static constexpr size_t underlying_value_to_type_index(underlying_type v) {
         for (const auto& item : underlying_value_metadata()) {
            if (item.value == v) {
               return item.type_index;
            }
         }
         return std::numeric_limits<size_t>::max();
      }
   };
}