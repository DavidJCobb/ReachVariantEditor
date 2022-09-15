#pragma once
#include <algorithm>
#include <array>
#include <stdexcept>
#include "helpers/array/filter.h"
#include "helpers/array/map.h"
#include "helpers/polyfills/msvc/const_reference_nttp_decltype.h"
#include "helpers/string/strcmp.h"
#include "helpers/type_traits/is_std_array_of_type.h"
#include "helpers/list_items_are_unique.h"
#include "helpers/numeric_min_max.h"
#include "../member.h"
#include "./validity_checks.h"

namespace cobb::reflex4::impl::enumeration {
   template<const auto& MemberDefinitionList> requires cobb::is_std_array_of_type<std::decay_t<cobb::polyfills::msvc::nttp_decltype<MemberDefinitionList>>, member>
   struct member_list_from_array {
      private:
         static constexpr auto& members = MemberDefinitionList;

      public:
         struct value_info {
            const char*  primary_name = nullptr;
            member_value value = {};
         };

         struct alternate_name {
            const char*  name  = nullptr;
            member_value value = {};
         };

      protected:
         static constexpr const auto _raw_value_info_list = []() consteval {
            std::array<value_info, std::tuple_size_v<std::decay_t<decltype(members)>>> out = {};

            member_value v = 0;
            for (size_t i = 0; i < out.size(); ++i) {
               const auto& item = members[i];
               if (item.value.has_value()) {
                  v = item.value.value();
               }
               out[i].primary_name = item.name;
               out[i].value        = v;
               ++v;
            }
            return out;
         }();
         static constexpr const auto _raw_unique_value_list = cobb::array::filter<
            _raw_value_info_list,
            [](const value_info& s, size_t i) -> bool {
               if (i == 0)
                  return true;
               for (size_t j = i - 1; j < i; ++j)
                  if (_raw_value_info_list[j].value == s.value)
                     return false;
               return true;
            }
         >;

      public:
         static constexpr const auto value_infos = []() consteval {
            auto unique = cobb::array::filter<
               _raw_value_info_list,
               [](const value_info& item, size_t i) -> bool {
                  if (i == 0)
                     return true;
                  for (size_t j = i - 1; j < i; ++j)
                     if (_raw_value_info_list[j].value == item.value)
                        return false;
                  return true;
               }
            >;
            std::sort(unique.begin(), unique.end(), [](const value_info& a, const value_info& b) { return a.value < b.value; });
            return unique;
         }();
         static constexpr const auto alternate_names = []() consteval {
            constexpr size_t count = _raw_value_info_list.size() - value_infos.size();

            std::array<alternate_name, count> out = {};

            size_t i = 0;
            for (const auto& item : _raw_value_info_list) {
               bool preserved = false;
               for (const auto& unique : value_infos) {
                  if (unique.value != item.value)
                     continue;
                  if (unique.primary_name == item.primary_name || cobb::strcmp(unique.primary_name, item.primary_name) == 0) {
                     preserved = true;
                     break;
                  }
               }
               if (!preserved) {
                  out[i] = {
                     .name  = item.primary_name,
                     .value = item.value,
                  };
                  ++i;
               }
            }
            return out;
         }();

      public:
         static constexpr const size_t value_count = value_infos.size();

         static constexpr const auto all_names = cobb::array::map(members, [](const member& item) { return item.name; });
         static constexpr const auto all_underlying_values = cobb::array::map(value_infos, [](const value_info& item) { return item.value; });

         static consteval const member_value max_underlying_value() requires(value_count > 0) {
            return cobb::max(all_underlying_values);
         }
         static consteval const member_value min_underlying_value() requires(value_count > 0) {
            return cobb::min(all_underlying_values);
         }

         static constexpr validity_check_results_t validity_check_results = {
            .names_are_not_blank =
               []() consteval -> bool {
                  for (const auto& item : _raw_value_info_list)
                     if (cobb::strcmp(item.primary_name, ""))
                        return false;
                  return true;
               }(),
            .names_are_unique = cobb::list_items_are_unique(_raw_value_info_list, [](const auto& a, const auto& b) { return cobb::strcmp(a.primary_name, b.primary_name) != 0; }),
         };
         
      public:
         static consteval bool has(const char* name) {
            for (const auto& item : members)
               if (cobb::strcmp(name, item.name) == 0)
                  return true;
            return false;
         }

         static consteval bool is_range_member(const char* name) {
            if (!has(name))
               throw std::invalid_argument("no member with this name exists");
            return false;
         }

         static consteval member_value underlying_value_of(const char* name) {
            if (!has(name))
               throw std::invalid_argument("no member with this name exists");
            for (const auto& info : value_infos)
               if (cobb::strcmp(name, info.primary_name) == 0)
                  return info.value;
            for (const auto& info : alternate_names)
               if (cobb::strcmp(name, info.name) == 0)
                  return info.value;
            throw std::logic_error("unreachable");
         }
         static consteval member_value underlying_value_of(const char* name, size_t range_index) {
            if (!has(name))
               throw std::invalid_argument("no member with this name exists");
            throw std::invalid_argument("this member is not a range");
         }

         static constexpr const char* name_of_value(member_value v) {
            for (const auto& info : value_infos)
               if (v == info.value)
                  return info.primary_name;
            return nullptr;
         }
   };
}