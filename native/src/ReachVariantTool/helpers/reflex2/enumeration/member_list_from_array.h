#pragma once
#include <array>
#include "helpers/string/strcmp.h"
#include "../member.h"
#include "./enumeration_data_forward_declare.h"
#include "./extract_underlying_type.h"

namespace cobb::reflex2 {
   namespace impl::enumeration {
      template<class Enumeration, typename DefinitionListType> struct member_list;
   }
}

namespace cobb::reflex2::impl::enumeration {
   template<class Enumeration, size_t Size> struct member_list<Enumeration, std::array<member, Size>> {
      private:
         static constexpr auto& members = enumeration_data<Enumeration>::members;

      public:
         using underlying_type = extract_underlying_type<Enumeration>::type;

      public:
         static constexpr size_t member_count = members.size();

         static constexpr auto all_names = [](){
            std::array<const char*, member_count> out = {};

            for (size_t i = 0; i < member_count; ++i)
               out[i] = members[i].name;

            return out;
         }();
         
         static constexpr auto all_underlying_values = [](){
            std::array<underlying_type, member_count> out = {};

            underlying_type v = 0;
            for (size_t i = 0; i < member_count; ++i) {
               const auto& item = members[i];
               if (item.value.has_value()) {
                  v = static_cast<underlying_type>(item.value.value());
               }
               out[i] = v;
               ++v;
            }

            return out;
         }();
         

         static constexpr bool any_values_unrepresentable = []() consteval -> bool {
            member_value v = 0;
            for (size_t i = 0; i < member_count; ++i) {
               const auto& item = members[i];
               if (item.value.has_value()) {
                  v = item.value.value();
                  if (static_cast<underlying_type>(v) != v)
                     return true;
               } else {
                  if (static_cast<underlying_type>(v) != v)
                     return true;
               }
               ++v;
            }
            return false;
         }();

         // optimization for retrieving names
         static constexpr bool _strictly_ascending_order = []() consteval {
            std::optional<underlying_type> last;
            for (const auto& item : members) {
               if (item.value.has_value()) {
                  if (!last.has_value()) {
                     last = (underlying_type)item.value.value();
                     continue;
                  }
                  if (item.value.value() < last.value()) {
                     return false;
                  }
               }
            }
            return true;
         }();

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

         static consteval underlying_type underlying_value_of(const char* name) {
            underlying_type v = 0;
            for (const auto& item : members) {
               if (item.value.has_value()) {
                  v = static_cast<underlying_type>(item.value.value());
               }
               if (cobb::strcmp(name, item.name) == 0) {
                  return v;
               }
               ++v;
            }
            throw std::invalid_argument("no member with this name exists");
         }
         static consteval underlying_type underlying_value_of(const char* name, size_t range_index) {
            if (!has(name))
               throw std::invalid_argument("no member with this name exists");
            throw std::invalid_argument("this member is not a range");
         }

         static constexpr const char* name_of_value(underlying_type v) {
            static_assert(all_underlying_values.size() == all_names.size());

            size_t i = 0;
            for (; i < all_underlying_values.size(); ++i) {
               auto valid = all_underlying_values[i];
               if (v == valid) {
                  break;
               }
               if constexpr (_strictly_ascending_order) {
                  if (v < valid) {
                     return nullptr;
                  }
               }
            }
            if (i >= all_underlying_values.size())
               return nullptr;

            return all_names[i];
         }
   };
}