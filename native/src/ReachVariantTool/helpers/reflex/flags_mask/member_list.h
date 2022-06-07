#pragma once
#include <array>
#include <tuple>
#include <type_traits>
#include "helpers/numeric_min_max.h"
#include "helpers/tuple_filter.h"
#include "helpers/tuple_foreach.h"
#include "helpers/tuple_index_of.h"
#include "helpers/unreachable.h"
#include "../member_concepts/named.h"
#include "../member_concepts/type_has_metadata.h"
#include "../member.h"
#include "../member_range.h"
#include "../nested_enum.h"
#include "./member_concepts.h"

namespace cobb::reflex::impl::flags_mask {
   template<typename... Members> struct member_list {
      using as_tuple = std::tuple<Members...>;

      struct entry_details {
         size_t flag_index;
         size_t type_index;
         size_t type_sub = 0;
         bool   has_metadata = false;
         #pragma warning(suppress: 26495)
      };
         
      static constexpr size_t flag_count() {
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
            cobb::unreachable();
         });
         return count;
      };
      static constexpr size_t type_count() { // including gaps
         return std::tuple_size_v<as_tuple>;
      }
      static constexpr size_t named_member_count() {
         return std::tuple_size_v<
            cobb::tuple_filter_t<
               []<typename Current>(){ return member_concepts::named<Current>; },
               as_tuple
            >
         >;
      }

      static constexpr auto all_entry_details = []() {
         std::array<entry_details, flag_count()> out = {};
         //
         size_t flag_index  = -1;
         size_t type_index  = -1;
         size_t value_index = -1;
         cobb::tuple_foreach<as_tuple>([&flag_index, &type_index, &value_index, &out]<typename Current>() {
            ++type_index;
            if constexpr (std::is_same_v<Current, member_gap>) {
               ++flag_index;
               return;
            }
            //
            if constexpr (is_member<Current> || is_member_range<Current>) {
               if (Current::value != undefined)
                  flag_index = Current::value;
               else
                  ++flag_index;
               //
               if constexpr (is_member<Current>) {
                  out[++value_index] = {
                     .flag_index   = flag_index,
                     .type_index   = type_index,
                     .has_metadata = member_concepts::type_has_metadata<Current>,
                  };
               } else if constexpr (is_member_range<Current>) {
                  for (size_t i = 0; i < Current::count; ++i) {
                     out[++value_index] = {
                        .flag_index   = flag_index,
                        .type_index   = type_index,
                        .type_sub     = i,
                        .has_metadata = member_concepts::type_has_metadata<Current>,
                     };
                     ++flag_index;
                  }
                  --flag_index;
               }
               return;
            }
         });
         //
         return out;
      }();

      // ---

      static constexpr size_t lowest_flag_index() {
         size_t i = std::numeric_limits<size_t>::max();
         for (auto& details : all_entry_details)
            if (i > details.flag_index)
               i = details.flag_index;
         return i;
      }
      static constexpr size_t highest_flag_index() {
         size_t i = 0;
         for (auto& details : all_entry_details)
            if (i < details.flag_index)
               i = details.flag_index;
         return i;
      }

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

      template<cobb::cs Name> static constexpr size_t base_flag_index_of() {
         constexpr auto index = member_list::template index_of<Name>();
         for (const auto& item : member_list::all_entry_details)
            if (item.type_index == index)
               return item.flag_index;
         cobb::unreachable();
      }

      // ---

      static constexpr std::array<const char*, named_member_count()> all_names() {
         std::array<const char*, named_member_count()> out = {};
         //
         size_t i = 0;
         cobb::tuple_foreach<as_tuple>([&i, &out]<typename Current>() {
            if constexpr (member_concepts::named<Current>) {
               out[i++] = Current::name.c_str();
            }
         });
         return out;
      };

      using metadata_having_members = cobb::tuple_filter_t<
         []<typename Current>() { return member_concepts::type_has_metadata<Current>; },
         as_tuple
      >;
      static constexpr bool uniform_metadata_types = [](){
         if constexpr (std::tuple_size_v<metadata_having_members> == 0) {
            return false;
         }
         //
         bool same = true;
         cobb::tuple_foreach<metadata_having_members>([&same]<typename A>() {
            if (!same)
               return;
            cobb::tuple_foreach<metadata_having_members>([&same]<typename B>() {
               if constexpr (std::is_same_v<A, B>)
                  return;
               if (!same)
                  return;
               same = std::is_same_v<typename A::metadata_type, typename B::metadata_type>;
            });
         });
         return same;
      }();

      template<typename Tuple> struct extract_metadata_type {
         using type = no_member_metadata;
      };
      template<typename Tuple> requires (std::tuple_size_v<Tuple> > 0) struct extract_metadata_type<Tuple> {
         using type = typename std::tuple_element_t<0, Tuple>::metadata_type;
      };
      using metadata_type = extract_metadata_type<
         std::conditional_t<
            uniform_metadata_types,
            metadata_having_members,
            std::tuple<>
         >
      >::type;

      // Indexed by type index
      static constexpr std::array<metadata_type, type_count()> all_metadata() {
         std::array<metadata_type, type_count()> out = {};
         if constexpr (uniform_metadata_types) {
            size_t i = 0;
            cobb::tuple_foreach<as_tuple>([&i, &out]<typename Current>(){
               if constexpr (member_concepts::type_has_metadata<Current>) {
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
      }
   };
}