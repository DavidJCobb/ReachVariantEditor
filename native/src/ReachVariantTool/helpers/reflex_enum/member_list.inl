#pragma once
#include "./member_list.h"
#include "./constants.h"

#define CLASS_TEMPLATE_PARAMS template<typename... Members> requires (is_member_type<Members> && ...)
#define CLASS_NAME member_list<Members...>
//
namespace cobb {
   namespace impl::reflex_enum {
      /*
      CLASS_TEMPLATE_PARAMS
      template<typename Functor> constexpr size_t CLASS_NAME::index_of_matching(Functor&& f) {
         size_t index = 0;
         (
            (f.template operator()<Members>() ?
               false             // if match: set result to current index; use false to short-circuit the "and" operator and stop iteration
            :
               ((++index), true) // no match: increment current index; use true to avoid short-circuiting, and continue iteration
            )
            && ...
         );
         return index < count ? index : index_of_none;
      }
      //*/
   }
}
//
#undef CLASS_TEMPLATE_PARAMS
#undef CLASS_NAME