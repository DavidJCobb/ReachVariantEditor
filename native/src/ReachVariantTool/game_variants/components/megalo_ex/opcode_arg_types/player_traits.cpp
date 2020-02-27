#pragma once
#include "player_traits.h"
#include "../opcode_arg.h"
#include "../../megalo/limits.h"
#include "../../../helpers/strings.h"
#include "../../../types/multiplayer.h" // GameVariantDataMultiplayer

namespace {
   constexpr int index_bits = cobb::bitcount(Megalo::Limits::max_script_traits - 1);
}
namespace MegaloEx {
   namespace types {
      //
      // FUNCTOR IMPLEMENTATION NOTES:
      //
      //  - This type stores a reference to one relevant-object.
      //
      OpcodeArgTypeinfo player_traits = OpcodeArgTypeinfo(
         QString("Player Traits"),
         QString("Megalo-defined sets of player traits. In Megalo, you don't \"apply\" and \"remove\" traits; rather, you call the \"apply\" function every frame, and the traits vanish when you stop doing that."),
         OpcodeArgTypeinfo::flags::may_need_postprocessing,
         //
         [](arg_functor_state fs, cobb::bitarray128& data, arg_rel_obj_list_t& relObjs, cobb::uint128_t input_bits) { // loader
            relObjs.ranges[fs.obj_index].start = data.size;
            relObjs.ranges[fs.obj_index].count = index_bits;
            data.consume(input_bits, index_bits);
            return true;
         },
         [](arg_functor_state fs, cobb::bitarray128& data, arg_rel_obj_list_t& relObjs, GameVariantData* vd) { // postprocess
            auto mp = dynamic_cast<GameVariantDataMultiplayer*>(vd);
            if (!mp)
               return false;
            uint16_t index = OpcodeArgValue::excerpt_loaded_index(data, relObjs, fs.obj_index);
            auto& traits = mp->scriptData.traits;
            if (traits.size() <= index)
               return false;
            relObjs.pointers[fs.obj_index] = &traits[index];
            return true;
         },
         [](arg_functor_state fs, cobb::bitarray128& data, arg_rel_obj_list_t& relObjs, std::string& out) { // to english
            auto obj = relObjs.pointers[fs.obj_index].pointer_cast<ReachMegaloPlayerTraits>();
            if (obj) {
               if (obj->name) {
                  out = obj->name->english();
                  return true;
               }
               cobb::sprintf(out, "script traits %u", obj->index);
               return true;
            }
            uint16_t index = OpcodeArgValue::excerpt_loaded_index(data, relObjs, fs.obj_index);
            cobb::sprintf(out, "missing traits %u", index);
            return true;
         },
         [](arg_functor_state fs, cobb::bitarray128& data, arg_rel_obj_list_t& relObjs, std::string& out) { // to script code
            uint16_t index = OpcodeArgValue::excerpt_loaded_index(data, relObjs, fs.obj_index);
            cobb::sprintf(out, "player_traits[%u]", index);
            return true;
         },
         nullptr // TODO: "compile" functor
      );
   }
}