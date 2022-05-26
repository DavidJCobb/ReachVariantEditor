#pragma once
#include <array>
#include <limits>
#include <tuple>
#include "./constants.h"
#include "./cs.h"
#include "./member.h"

namespace cobb {
   namespace impl::reflex_enum {
      template<typename... Members> requires (is_member_type<Members> && ...) class member_list {
         public:
            static constexpr size_t index_of_none = std::numeric_limits<size_t>::max();

            static constexpr size_t count = sizeof...(Members);

            using as_tuple = std::tuple<Members...>;
            template<size_t i> requires (i < count) using nth_type = typename std::tuple_element_t<i, as_tuple>;

         private:
            member_list()  = delete;
            ~member_list() = delete;

         protected:
            template<typename A, typename... B> struct _all_member_names_unique {
               static constexpr bool value = ([]() {
                  bool a_overlaps = ((A::name == B::name) || ...);
                  if (a_overlaps)
                     return false;
                  if constexpr (sizeof...(B) > 1) {
                     return _all_member_names_unique<B...>::value;
                  }
                  return true;
               })();
            };
            template<typename Underlying, typename T> struct _member_value_representable {
               static constexpr bool value =
                  (T::value == impl::reflex_enum::undefined) || 
                  (
                     T::value >= std::numeric_limits<Underlying>::lowest()
                  && T::value <= std::numeric_limits<Underlying>::max()
                  );
            };

         public:
            static constexpr bool all_member_names_unique = ([]() {
               if constexpr (sizeof...(Members) > 1) {
                  return _all_member_names_unique<Members...>::value;
               }
               return true;
            })();
            template<typename Underlying> static constexpr bool all_explicit_member_values_representable = ([]() {
               return (_member_value_representable<Underlying, Members>::value && ...);
            })();

         public:
            template<typename Functor> static constexpr void for_each(Functor&& f) {
               (f.template operator()<Members>(), ...);
            }

            static constexpr std::array<const char*, count> all_names() {
               std::array<const char*, count> out = {};
               //
               size_t i = 0;
               for_each([&out, &i]<typename Current>() {
                  out[i++] = Current::name.c_str();
               });
               return out;
            };
            template<typename Underlying> static constexpr std::array<Underlying, count> all_underlying_values() {
               std::array<Underlying, count> out = {};

               Underlying v = -1;

               size_t i = 0;
               for_each([&out, &v, &i]<typename Current>() {
                  if constexpr (Current::value == undefined)
                     ++v;
                  else
                     v = (Underlying)Current::value;
                  out[i] = v;
                  ++i;
               });

               return out;
            };

            template<cs Name> static constexpr size_t index_of_name = ([]() {
               size_t index = 0;
               (
                  (Members::name == Name ?
                     false             // if match: set result to current index; use false to short-circuit the "and" operator and stop iteration
                  :
                     ((++index), true) // no match: increment current index; use true to avoid short-circuiting, and continue iteration
                  )
                  && ...
               );
               return index < count ? index : index_of_none;
            })();

            template<typename T> static constexpr size_t index_of = ([]() {
               size_t index = 0;
               (
                  (std::is_same_v<T, Members> ?
                     false             // if match: set result to current index; use false to short-circuit the "and" operator and stop iteration
                  :
                     ((++index), true) // no match: increment current index; use true to avoid short-circuiting, and continue iteration
                  )
                  && ...
               );
               return index < count ? index : std::numeric_limits<size_t>::max();
            })();

            template<typename Functor> static constexpr size_t index_of_matching(Functor&& f);

      };

      template<typename T> struct member_list_from_tuple {
         using type = void;
      };
      template<typename... Types> struct member_list_from_tuple<std::tuple<Types...>> {
         using type = member_list<Types...>;
      };
   }
}

#include "./member_list.inl"