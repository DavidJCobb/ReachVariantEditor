#pragma once
#include "./named.h"

namespace cobb::reflex::member_concepts {
   namespace impl {
      template<typename A, typename... B> class names_are_unique {
         protected:
            template<typename, typename> struct compare {
               static constexpr bool value = false;
            };
            template<named A, named B> struct compare<A, B> {
               static constexpr bool value = (A::name == B::name);
            };

         public:
            static constexpr bool value = ([]() {
               if constexpr (named<A>) {
                  bool a_overlaps = (compare<A, B>::value || ...);
                  if (a_overlaps)
                     return false;
               }
               if constexpr (sizeof...(B) > 1) {
                  return names_are_unique<B...>::value;
               }
               return true;
            })();
      };
      template<typename A> class names_are_unique<A> {
         public:
            static constexpr bool value = true;
      };
   }
   //
   template<typename... Types> concept names_are_unique = impl::names_are_unique<Types...>::value;
}
