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

      struct entry_details {
         underlying_type value;
         size_t type_index;
         size_t type_sub = 0;
         #pragma warning(suppress: 26495)
      };
         
      static constexpr size_t value_count = [](){
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
               using nested = Current::enumeration;
               count += (nested::max_underlying_value - nested::min_underlying_value) + 1;
               return;
            }
            cobb::unreachable();
         });
         return count;
      }();
      static constexpr size_t type_count = std::tuple_size_v<as_tuple>; // including gaps
      static constexpr size_t named_member_count = std::tuple_size_v<
         cobb::tuple_filter_t<
            []<typename Current>(){ return member_concepts::named<Current>; },
            as_tuple
         >
      >;

      static constexpr auto all_entry_details = [](){
         std::array<entry_details, value_count> out = {};
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
      }();

      //

      template<cs Name> static constexpr size_t index_of() {
         return cobb::tuple_index_of_matching<
            as_tuple,
            []<typename Current>() -> bool {
               if constexpr (member_concepts::named<Current>)
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

      static constexpr std::array<const char*, named_member_count> all_names() {
         std::array<const char*, named_member_count> out = {};
         //
         size_t i = 0;
         cobb::tuple_foreach<as_tuple>([&i, &out]<typename Current>() {
            if constexpr (member_concepts::named<Current>) {
               out[i++] = Current::name.c_str();
            }
         });
         return out;
      };
      static constexpr auto all_underlying_values = [](){
         std::array<underlying_type, all_entry_details.size()> out = {};
         for (size_t i = 0; i < all_entry_details.size(); ++i)
            out[i] = all_entry_details[i].value;
         return out;
      }();

      template<cobb::cs Name> static constexpr underlying_type underlying_value_of() {
         constexpr auto index = index_of<Name>();
         for (const auto& item : all_entry_details)
            if (item.type_index == index)
               return item.value;
         cobb::unreachable();
      }
      
      template<typename Subset> static constexpr size_t subset_name_length() {
         return std::tuple_element_t<index_of_subset<Subset>(), as_tuple>::name.capacity();
      }
      template<typename Subset> static constexpr cobb::cs<subset_name_length<Subset>()> subset_name() {
         constexpr auto length = subset_name_length<Subset>();
         //
         cobb::cs<length> name;
         cobb::tuple_foreach<as_tuple>([&name]<typename Current>() {
            if constexpr (is_nested_enum<Current>)
               name = Current::name;
         });
         return name;
      }

      static constexpr size_t underlying_value_to_type_index(underlying_type v) {
         for (const auto& item : all_entry_details)
            if (item.value == v)
               return item.type_index;
         return std::numeric_limits<size_t>::max();
      }

      using metadata_having_members = cobb::tuple_filter_t<
         []<typename Current>() { return member_type_has_metadata<Current>; },
         as_tuple
      >;
      template<typename Tuple> struct extract_metadata_type {
         using type = no_member_metadata;
      };
      template<typename Tuple> requires (std::tuple_size_v<Tuple> > 0) struct extract_metadata_type<Tuple> {
         using type = typename std::tuple_element_t<0, Tuple>::metadata_type;
      };
      static constexpr bool uniform_metadata_types = [](){
         using metadata_type = extract_metadata_type<metadata_having_members>::type;
         //;
         if constexpr (std::tuple_size_v<metadata_having_members> == 0) {
            return false;
         }
         return true;
         bool same = true;
         cobb::tuple_foreach<metadata_having_members>([&same]<typename Current>() {
            if (!same)
               return;
            if constexpr (!std::is_same_v<typename Current::metadata_type, metadata_type>) {
               same = false;
            }
         });
         return same;
      }();

      using metadata_type = extract_metadata_type<
         std::conditional_t<
            uniform_metadata_types,
            metadata_having_members,
            std::tuple<>
         >
      >::type;

      // Indexed by type index
      static constexpr auto all_metadata = [](){
         std::array<metadata_type, type_count> out = {};
         if constexpr (uniform_metadata_types) {
            size_t i = 0;
            cobb::tuple_foreach<as_tuple>([&i, &out]<typename Current>(){
               if constexpr (member_type_has_metadata<Current>) {
                  //out[i] = (metadata_type)Current::metadata;
                  // 
                  // IntelliSense thinks the array element is const, yet also doesn't fail static assertions 
                  // on it being non-const, so to avoid false-positive errors in the IDE, we have to do this 
                  // unnecessary garbage:
                  //
                  std::construct_at(&out[i], Current::metadata);
               }
               ++i;
            });
         }
         return out;
      }();
   };
}