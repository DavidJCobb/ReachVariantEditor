#pragma once
#include <array>
#include <tuple>
#include "helpers/string/strcmp.h"
#include "helpers/tuple/filter_values_by_type.h"
#include "helpers/tuple/index_of_matching_value.h"
#include "helpers/tuple_filter.h"
#include "helpers/tuple_foreach.h"
#include "helpers/tuple_for_each_value.h"
#include "helpers/unreachable.h"
#include "./concepts/is_named_member.h"
#include "./concepts/is_real_member.h"
#include "./concepts/is_valid_member_type.h"
#include "../member.h"
#include "../member_enum.h"
#include "../member_range.h"
#include "./enumeration_data_forward_declare.h"
#include "./extract_underlying_type.h"

namespace cobb::reflex2 {
   namespace impl::enumeration {
      template<class Enumeration, typename DefinitionListType> struct member_list;
   }
}

namespace cobb::reflex2::impl::enumeration {
   template<class Enumeration, typename... MemberDefTypes> struct member_list<Enumeration, std::tuple<MemberDefTypes...>> {
      public:
         using underlying_type = extract_underlying_type<Enumeration>::type;

      private:
         using tuple_type = std::tuple<MemberDefTypes...>;
         static constexpr auto& members = enumeration_data<Enumeration>::members;

         static_assert( // No unrecognized types in tuple
            []() -> bool {
               bool valid = true;
               cobb::tuple_foreach<tuple_type>([&valid]<typename T>() {
                  if (is_valid_member_type<T>) {
                     return;
                  }
                  valid = false;
               });
               return valid;
            }(),
            "The \"members\" tuple contains types unrelated to reflex enums."
         );

         struct member_details { // one entry per member; (member_range::count) entries per member_range; etc.
            underlying_type value;
            size_t member_index;   // also index in all_names
            size_t member_sub = 0; // for ranges and nested enums
            #pragma warning(suppress: 26495)
         };
         

      public:
         static constexpr size_t member_count = []() consteval {
            size_t count = 0;
            cobb::tuple_for_each_value(members, [&count]<typename Current>(const Current& item) {
               if constexpr (!is_real_member<Current>) {
                  return;
               } else if constexpr (std::is_same_v<Current, member>) {
                  ++count;
                  return;
               } else if constexpr (std::is_same_v<Current, member_range>) {
                  count += item.count;
                  return;
               } else if constexpr (is_member_enum<Current>) {
                  count += std::tuple_size_v<std::decay_t<decltype(Current::enumeration::all_underlying_values)>>;
                  return;
               } else {
                  cobb::unreachable();
               }
            });
            return count;
         }();
         static constexpr size_t name_count = std::tuple_size_v<
            cobb::tuple_filter_t<
               []<typename Current>(){ return is_named_member<Current>; },
               tuple_type
            >
         >;
         
         static constexpr auto all_names = []() {
            std::array<const char*, name_count> out = {};
            //
            size_t i = 0;
            cobb::tuple_for_each_value(members, [&i, &out]<typename Current>(const Current& item) {
               if constexpr (is_named_member<Current>) {
                  out[i++] = item.name;
               }
            });
            return out;
         }();
         static constexpr auto all_member_details = []() consteval {
            std::array<member_details, member_count> out = {};
            //
            underlying_type v   = -1;
            size_t member_index = -1;
            size_t value_index  = -1;
            cobb::tuple_for_each_value(members, [&v, &member_index, &value_index, &out]<typename Current>(const Current& item) {
               if constexpr (std::is_same_v<Current, member_gap>) {
                  ++v;
                  return;
               }
               ++member_index;
               //
               if constexpr (std::is_same_v<Current, member> || std::is_same_v<Current, member_range>) {
                  if (item.value.has_value())
                     v = static_cast<underlying_type>(item.value.value());
                  else
                     ++v;
                  //
                  if constexpr (std::is_same_v<Current, member>) {
                     out[++value_index] = {
                        .value        = v,
                        .member_index = member_index,
                     };
                  } else if constexpr (std::is_same_v<Current, member_range>) {
                     for (size_t i = 0; i < item.count; ++i) {
                        out[++value_index] = {
                           .value        = v,
                           .member_index = member_index,
                           .member_sub   = i,
                        };
                        ++v;
                     }
                     --v;
                  }
                  return;
               }
               if constexpr (is_member_enum<Current>) {
                  using nested = typename Current::enumeration;
                  auto  nested_span = (nested::max_underlying_value() - nested::min_underlying_value());
                  auto& nested_list = nested::all_underlying_values;
                  //
                  ++v;
                  for (size_t i = 0; i < nested_list.size(); ++i) {
                     auto item = nested_list[i];
                     //
                     out[++value_index] = {
                        .value        = (v + static_cast<underlying_type>(item - nested::min_underlying_value())),
                        .member_index = member_index,
                        .member_sub   = i,
                     };
                  }
                  v += static_cast<underlying_type>(nested_span);
                  v -= 1;
                  return;
               }
            });
            //
            return out;
         }();

         static constexpr bool any_values_unrepresentable = []() consteval -> bool {
            bool clipped = false;
            member_value v = 0;
            cobb::tuple_for_each_value(members, [&clipped, &v]<typename Current>(const Current& item) {
               auto _would_clip = [](member_value v) -> bool {
                  return v != (underlying_type)v;
               };

               if constexpr (std::is_same_v<Current, member_gap>) {
                  ++v;
                  return;
               }
               if constexpr (std::is_same_v<Current, member> || std::is_same_v<Current, member_range>) {
                  if (item.value.has_value())
                     v = item.value.value();
                  else
                     ++v;
                  if (_would_clip(v)) {
                     clipped = true;
                  }
                  //
                  if constexpr (std::is_same_v<Current, member_range>) {
                     if (_would_clip(v + item.count - 2)) {
                        clipped = true;
                     }
                  }
                  return;
               }
               if constexpr (is_member_enum<Current>) {
                  using nested = Current::enumeration;
                  constexpr const auto nested_span = (nested::max_underlying_value() - nested::min_underlying_value());

                  ++v;
                  if (_would_clip(v) || _would_clip(v + nested_span)) {
                     clipped = true;
                  }
                  v += nested_span - 1;
                  return;
               }
            });
            return clipped;
         }();
         
         static consteval size_t index_of(const char* name) {
            return cobb::tuple_index_of_matching_value(
               cobb::tuple_filter_values_by_type<[]<typename Current>(){ return is_named_member<Current>; }>(members),
               [name]<typename Current>(const Current& item) -> bool {
                  return cobb::strcmp(name, item.name) == 0;
               }
            );
         }

         static constexpr auto all_underlying_values = [](){
            std::array<underlying_type, all_member_details.size()> out = {};
            for (size_t i = 0; i < all_member_details.size(); ++i)
               out[i] = all_member_details[i].value;
            return out;
         }();

         //

         static consteval underlying_type max_underlying_value() requires(member_count > 0) {
            member_value n = all_underlying_values[0];
            for (const auto v : all_underlying_values)
               if (v > n)
                  n = v;
            return n;
         }
         static consteval underlying_type min_underlying_value() requires(member_count > 0) {
            member_value n = all_underlying_values[0];
            for (const auto v : all_underlying_values)
               if (v < n)
                  n = v;
            return n;
         }
         
      public:
         static consteval bool has(const char* name) {
            for (const auto* item_name : all_names)
               if (cobb::strcmp(name, item_name) == 0)
                  return true;
            return false;
         }

         static consteval size_t member_range_count(const char* name) {
            if (!has(name))
               throw std::invalid_argument("no member with this name exists");
            bool   found  = false;
            size_t result = 0;
            cobb::tuple_for_each_value(members, [name, &found, &result]<typename Current>(const Current& item) {
               if (found)
                  return;
               if constexpr (is_named_member<Current>) {
                  if (cobb::strcmp(name, item.name) == 0) {
                     found = true;
                     if constexpr (std::is_same_v<Current, member_range>) {
                        result = item.count;
                     }
                  }
               }
            });
            return result;
         }
         static consteval bool is_range_member(const char* name) {
            return member_range_count(name) != 0;
         }

         static consteval underlying_type underlying_value_of(const char* name) {
            auto index = index_of(name);
            for (const auto& item : all_member_details)
               if (item.member_index == index)
                  return item.value;
            throw std::invalid_argument("no member with this name exists");
         }
         static consteval underlying_type underlying_value_of(const char* name, size_t range_index) {
            auto count = member_range_count(name);
            if (count == 0)
               throw std::invalid_argument("this member is not a range");
            if (range_index >= count)
               throw std::invalid_argument("range-index out of range");
            return underlying_value_of(name) + range_index;
         }

         static constexpr const char* name_of_value(underlying_type v) {
            for (const auto& item : all_member_details) {
               if (item.value == v) {
                  return all_names[item.member_index];
               }
            }
            return nullptr;
         }
   };
}