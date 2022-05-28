#pragma once
#include <array>
#include <limits>
#include <tuple>
#include "helpers/tuple_filter.h"
#include "helpers/tuple_foreach.h"
#include "helpers/tuple_index_of.h"
#include "helpers/tuple_unpack.h"
#include "../cs.h"
#include "./constants.h"
#include "./member.h"

namespace cobb {
   namespace impl::reflex_enum {
      template<typename... Members> requires ((is_member_type<Members> || std::is_same_v<Members, reflex_enum_gap>) && ...) class member_list {
         public:
            static constexpr size_t index_of_none = std::numeric_limits<size_t>::max();

         protected:
            static constexpr size_t count_including_gaps = sizeof...(Members);

            using as_tuple_with_gaps = std::tuple<Members...>;

            template<size_t i> requires (i < count_including_gaps) using nth_type_with_gaps = typename std::tuple_element_t<i, as_tuple_with_gaps>;

         public:
            using as_tuple = tuple_filter_t<
               []<typename T>() {
                  return !std::is_same_v<T, reflex_enum_gap>;
               },
               as_tuple_with_gaps
            >;
            static constexpr size_t count = std::tuple_size_v<as_tuple>;
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
            template<typename Underlying, typename T> requires (is_member_type<T> || std::is_same_v<T, reflex_enum_gap>) struct _member_value_representable {
               static constexpr bool value = ([]() {
                  if constexpr (std::is_same_v<T, reflex_enum_gap>) {
                     return true;
                  } else {
                     if (T::value == impl::reflex_enum::undefined)
                        return true;
                     //
                     using limits = std::numeric_limits<Underlying>;
                     if (T::value < limits::lowest())
                        return false;
                     if (T::value > limits::max())
                        return false;
                     return true;
                  }
               })();
            };

         public:
            static constexpr bool all_member_names_unique = ([]() {
               if constexpr (std::tuple_size_v<as_tuple> > 1)
                  return cobb::tuple_unpack_t<_all_member_names_unique, as_tuple>::value;
               return true;
            })();
            template<typename Underlying> static constexpr bool all_explicit_member_values_representable = ([]() {
               return (_member_value_representable<Underlying, Members>::value && ...);
            })();

         public:
            template<typename Functor> static constexpr void for_each(Functor&& f) {
               cobb::tuple_foreach<as_tuple>(std::forward<Functor&&>(f));
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
               //
               Underlying v = -1;
               size_t     i = 0;
               cobb::tuple_foreach<as_tuple_with_gaps>([&out, &v, &i]<typename Current>() {
                  if constexpr (std::is_same_v<Current, reflex_enum_gap>) {
                     ++v;
                  } else {
                     if constexpr (Current::value == undefined)
                        ++v;
                     else
                        v = (Underlying)Current::value;
                     out[i] = v;
                     ++i;
                  }
               });
               //
               return out;
            };

            template<typename T> static constexpr size_t index_of = ([]() {
               return cobb::tuple_index_of<T, as_tuple>;
            })();
            template<cs Name> static constexpr size_t index_of_name() { return cobb::tuple_index_of_matching<
               as_tuple,
               []<typename Current>() -> bool {
                  return Current::name == Name;
               }
            >(); }

            template<typename Functor> static constexpr size_t index_of_matching(Functor&& f) {
               return cobb::tuple_index_of_matching<as_tuple>(std::forward<Functor&&>(f));
            }
      };

      template<typename T> struct member_list_from_tuple {
         using type = void;
      };
      template<typename... Types> struct member_list_from_tuple<std::tuple<Types...>> {
         using type = member_list<Types...>;
      };
   }
}