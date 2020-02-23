#pragma once
#include "forge_label.h"
#include "../opcode_arg.h"
#include "../../megalo/forge_label.h"
#include "../../megalo/limits.h"
#include "../../../helpers/strings.h"
#include "../../../types/multiplayer.h" // GameVariantDataMultiplayer

namespace {
   constexpr int index_bits = cobb::bitcount(Megalo::Limits::max_script_labels - 1);
}
namespace MegaloEx {
   namespace types {
      //
      // FUNCTOR IMPLEMENTATION NOTES:
      //
      //  - The (fragment) argument is used as an index into the (relObjs) list.
      //
      //  - Output functors read the bit buffer from the start (offset 0). If you need to call these 
      //    functors from another functor (i.e. if there is some argument type that includes this 
      //    type inside of it), then you must pass a copy of the bitarray shifted appropriately.
      //
      OpcodeArgTypeinfo forge_label = OpcodeArgTypeinfo(
         QString("Forge Label"),
         QString(""),
         OpcodeArgTypeinfo::flags::may_need_postprocessing,
         //
         [](uint8_t fragment, cobb::bitarray128& data, arg_rel_obj_list_t& relObjs, cobb::uint128_t input_bits) { // loader
            if (data.consume(input_bits, 1) == 0) // absence bit; 0 means we have a value
               data.consume(input_bits, index_bits);
            return true;
         },
         [](uint8_t fragment, cobb::bitarray128& data, arg_rel_obj_list_t& relObjs, GameVariantData* vd) { // postprocess
            bool absence = data.size == 1;
            if (absence)
               return true;
            auto mp = dynamic_cast<GameVariantDataMultiplayer*>(vd);
            if (!mp)
               return false;
            uint16_t index = data.excerpt(1, index_bits);
            auto& list = mp->scriptContent.forgeLabels;
            if (list.size() <= index)
               return false;
            relObjs[fragment] = &list[index];
            return true;
         },
         [](uint8_t fragment, cobb::bitarray128& data, arg_rel_obj_list_t& relObjs, std::string& out) { // to english
            auto obj = relObjs[fragment].pointer_cast<Megalo::ReachForgeLabel>();
            if (obj) {
               if (obj->name) {
                  out = obj->name->english();
                  return true;
               }
               cobb::sprintf(out, "unnamed Forge label %u", obj->index);
               return true;
            }
            uint16_t index = data.excerpt(1, index_bits);
            cobb::sprintf(out, "missing Forge label %u", index);
            return true;
         },
         [](uint8_t fragment, cobb::bitarray128& data, arg_rel_obj_list_t& relObjs, std::string& out) { // to script code
            auto obj = relObjs[fragment].pointer_cast<Megalo::ReachForgeLabel>();
            if (obj) {
               if (obj->name) {
                  cobb::sprintf(out, "\"%s\"", obj->name->english()); // TODO: this will break if the name contains double-quotes
                  return true;
               }
               cobb::sprintf(out, "%u", obj->index);
               return true;
            }
            uint16_t index = data.excerpt(1, index_bits);
            cobb::sprintf(out, "%u", index);
            return true;
         },
         nullptr // TODO: "compile" functor
      );
   }
}