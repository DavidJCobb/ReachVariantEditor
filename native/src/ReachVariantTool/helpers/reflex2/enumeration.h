#pragma once
#include <stdexcept>
#include <utility>
#include "../string/strcmp.h"
#include "./member.h"

namespace cobb::reflex2 {
   template<class Enumeration> struct enumeration_members;

   template<class Subclass> class enumeration {
      using my_type = Subclass;

      static constexpr auto& members = enumeration_members<my_type>::list;

      protected:
         member_value _value = 0;

      public:
         static constexpr auto all_underlying_values = [](){
            std::array<member_value, members.size()> out = {};

            member_value v = 0;
            for (size_t i = 0; i < members.size(); ++i) {
               const auto& item = members[i];
               if (item.value.has_value()) {
                  v = item.value.value();
               }
               out[i] = v;
               ++v;
            }

            return out;
         }();
         static constexpr size_t member_count = members.size();

      public:
         static consteval member_value underlying_value_of(const char* name) {
            member_value v = 0;
            for (const auto& item : members) {
               if (item.value.has_value()) {
                  v = item.value.value();
               }
               if (cobb::strcmp(name, item.name) == 0) {
                  return v;
               }
               ++v;
            }
            throw std::invalid_argument("no member with this name exists");
         }
   };
}