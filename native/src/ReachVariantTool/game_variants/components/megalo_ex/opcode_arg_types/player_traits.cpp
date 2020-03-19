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
         [](arg_functor_state fs, OpcodeArgValue& arg, cobb::uint128_t input_bits) { // loader
            auto& data = arg.data;
            auto& ro   = arg.relevant_objects;
            ro.ranges[fs.obj_index].start = data.size;
            ro.ranges[fs.obj_index].count = index_bits;
            data.consume(input_bits, index_bits);
            return true;
         },
         [](arg_functor_state fs, OpcodeArgValue& arg, GameVariantData* vd) { // postprocess
            auto& data = arg.data;
            auto& ro   = arg.relevant_objects;
            auto mp = dynamic_cast<GameVariantDataMultiplayer*>(vd);
            if (!mp)
               return OpcodeArgTypeinfo::functor_failed;
            uint16_t index = arg.excerpt_loaded_index(fs.obj_index);
            auto& traits = mp->scriptData.traits;
            if (traits.size() <= index)
               return OpcodeArgTypeinfo::functor_failed;
            ro.pointers[fs.obj_index] = &traits[index];
            return (int32_t)ro.ranges[fs.obj_index].count;
         },
         [](arg_functor_state fs, OpcodeArgValue& arg, std::string& out) { // to english
            auto& ro  = arg.relevant_objects;
            auto  obj = ro.pointers[fs.obj_index].pointer_cast<ReachMegaloPlayerTraits>();
            if (obj) {
               if (obj->name) {
                  out = obj->name->english();
                  return ro.ranges[fs.obj_index].count;
               }
               cobb::sprintf(out, "script traits %u", obj->index);
               return ro.ranges[fs.obj_index].count;
            }
            uint16_t index = arg.excerpt_loaded_index(fs.obj_index);
            cobb::sprintf(out, "missing traits %u", index);
            return ro.ranges[fs.obj_index].count;
         },
         [](arg_functor_state fs, OpcodeArgValue& arg, std::string& out) { // to script code
            uint16_t index = arg.excerpt_loaded_index(fs.obj_index);
            cobb::sprintf(out, "player_traits[%u]", index);
            return arg.relevant_objects.ranges[fs.obj_index].count;
         },
         nullptr // TODO: "compile" functor
      );
   }
}