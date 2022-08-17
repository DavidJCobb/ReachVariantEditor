#include "./phrase_tree.h"

namespace halo::reach::megalo::bolt::util {

   using phrase_type = phrase_tree<
      cobb::cs("abc"),
      cobb::cs("def jkl"),
      cobb::cs("def ghi")
   >;

   constexpr auto test = phrase_type{};
   constexpr auto tops = test.top_level_nodes;

   constexpr auto blob = phrase_type::phrase_blob;

   constexpr auto nodes = test.all_nodes[1].data;
   static_assert(test.all_nodes[0].data.start == 0);
   static_assert(test.all_nodes[0].data.size  == 3);
   static_assert(test.all_nodes[1].data.start == 3);
   static_assert(test.all_nodes[1].data.size  == 3);
}