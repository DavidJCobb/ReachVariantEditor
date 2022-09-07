#pragma once
#include <tuple>
#include "helpers/tuple/contains_type.h"
#include "helpers/tuple/filter_types.h"
#include "helpers/type_traits/is_std_tuple.h"
#include "helpers/tuple_foreach.h"
#include "helpers/tuple_transform.h"
#include "./enumeration_data_forward_declare.h"
#include "../member.h"

namespace cobb::reflex3::impl::enumeration {
   template<typename T> concept can_nest_enums = requires {
      typename enumeration_data<T>;
      enumeration_data<T>::members;
      requires cobb::is_std_tuple<decltype(enumeration_data<T>::members)>;
   };
   template<class Enumeration> struct nested_enums_within {
      using types = std::tuple<>;
   };
   template<class Enumeration> requires (can_nest_enums<Enumeration>) struct nested_enums_within<Enumeration> {
      template<typename T> struct transform {
         using type = T::enumeration;
      };

      using types = 
         cobb::tuple_transform<
            transform,
            cobb::tuple_filter_types<decltype(enumeration_data<Enumeration>::members), []<typename T>(){ return is_member_enum<T>; }>
         >;
   };

   namespace impl::is_cyclically_nested {
      template<typename T> concept can_nest_enums = requires {
         typename enumeration_data<T>;
         enumeration_data<T>::members;
         requires cobb::is_std_tuple<decltype(enumeration_data<T>::members)>;
      };

      template<typename Subject, typename Start> struct result;

      template<typename Subject, typename Start> requires (!can_nest_enums<Subject>)
      struct result<Subject, Start> {
         static constexpr bool cyclical = false;
      };

      template<typename Subject, typename Start> requires (can_nest_enums<Subject>)
      struct result<Subject, Start> {
         static constexpr bool cyclical =
            cobb::tuple::contains_type<nested_enums_within<Subject>::types, Start> ||
            (
               std::tuple_size_v<
                  cobb::tuple_filter_types<
                     nested_enums_within<Subject>::types,
                     []<typename Nested>() { return !result<Nested, Start>::cyclical; }
                  >
               > != 0
            );
      };
   }

   template<class Enumeration> constexpr bool is_cyclically_nested = impl::is_cyclically_nested::result<Enumeration, Enumeration>::cyclical;
}
