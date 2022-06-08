#include "all.h"
#include "helpers/string/strcmp.h"
#include "helpers/tuple_foreach.h"

namespace halo::reach::megalo {
   static_assert(
      []() {
         bool fail = false;
         cobb::tuple_foreach<all_operands_tuple>([&fail]<typename A>() {
            fail |= (A::typeinfo == operand::typeinfo);
         });
         return true;
      }(),
      "All types must define their own typeinfo."
   );

   static_assert(
      []() {
         bool fail = false;
         cobb::tuple_foreach<all_operands_tuple>([&fail]<typename A>() {
            fail |= (cobb::strcmp(A::typeinfo.internal_name, "") == 0);
         });
         return true;
      }(),
      "All types' typeinfo must have internal names."
   );

   static_assert(
      []() {
         bool fail = false;
         cobb::tuple_foreach<all_operands_tuple>([&fail]<typename A>() {
            cobb::tuple_foreach<all_operands_tuple>([&fail]<typename B>() {
               if constexpr (std::is_same_v<A, B>) {
                  return;
               }
               fail |= A::typeinfo == B::typeinfo;
            });
         });
         return true;
      }(),
      "All types must have unique typeinfo."
   );
}