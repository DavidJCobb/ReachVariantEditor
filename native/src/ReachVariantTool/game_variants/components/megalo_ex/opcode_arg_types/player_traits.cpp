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
      OpcodeArgTypeinfo player_traits = OpcodeArgTypeinfo(
         QString("Player Traits"),
         QString("Megalo-defined sets of player traits. In Megalo, you don't \"apply\" and \"remove\" traits; rather, you call the \"apply\" function every frame, and the traits vanish when you stop doing that."),
         OpcodeArgTypeinfo::flags::may_need_postprocessing,
         //
         [](OpcodeArgValue& instance, uint64_t bits) { // loader
            instance.data.consume(bits, index_bits);
            return true;
         },
         [](OpcodeArgValue& instance, GameVariantData* vd) { // postprocess
            auto mp = dynamic_cast<GameVariantDataMultiplayer*>(vd);
            if (!mp)
               return false;
            uint16_t index = instance.data.excerpt(0, index_bits);
            auto& traits = mp->scriptData.traits;
            if (traits.size() <= index)
               return false;
            instance.relevant_objects[0] = &traits[index];
            return true;
         },
         [](OpcodeArgValue& instance, std::string& out) { // to english
            if (instance.relevant_objects[0]) {
               auto f = (ReachMegaloPlayerTraits*)(cobb::reference_tracked_object*)instance.relevant_objects[0];
               if (f->name) {
                  out = f->name->english();
                  return true;
               }
               cobb::sprintf(out, "script traits %u", f->index);
               return true;
            }
            uint16_t index = instance.data.excerpt(0, index_bits);
            cobb::sprintf(out, "missing traits %u", index);
            return true;
         },
         [](OpcodeArgValue& instance, std::string& out) { // to script code
            uint16_t index = instance.data.excerpt(0, index_bits);
            cobb::sprintf(out, "player_traits[%u]", index);
            return true;
         },
         nullptr // TODO: "compile" functor
      );
   }
}