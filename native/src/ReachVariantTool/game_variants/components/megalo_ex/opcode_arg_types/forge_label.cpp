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
      OpcodeArgTypeinfo forge_label = OpcodeArgTypeinfo(
         QString("Forge Label"),
         QString(""),
         OpcodeArgTypeinfo::flags::may_need_postprocessing,
         //
         [](OpcodeArgValue& instance, uint64_t bits) { // loader
            if (instance.data.consume(bits, 1) == 0) // absence bit; 0 means we have a value
               instance.data.consume(bits, index_bits);
            return true;
         },
         [](OpcodeArgValue& instance, GameVariantData* vd) { // postprocess
            bool absence = instance.data.size == 1;
            if (absence)
               return true;
            auto mp = dynamic_cast<GameVariantDataMultiplayer*>(vd);
            if (!mp)
               return false;
            uint16_t index = instance.data.excerpt(1, index_bits);
            auto& list = mp->scriptContent.forgeLabels;
            if (list.size() <= index)
               return false;
            instance.relevant_objects[0] = &list[index];
            return true;
         },
         [](OpcodeArgValue& instance, std::string& out) { // to english
            if (instance.relevant_objects[0]) {
               auto f = (Megalo::ReachForgeLabel*)(cobb::reference_tracked_object*)instance.relevant_objects[0];
               if (f->name) {
                  out = f->name->english();
                  return true;
               }
               cobb::sprintf(out, "unnamed Forge label %u", f->index);
               return true;
            }
            uint16_t index = instance.data.excerpt(1, index_bits);
            cobb::sprintf(out, "missing Forge label %u", index);
            return true;
         },
         [](OpcodeArgValue& instance, std::string& out) { // to script code
            if (instance.relevant_objects[0]) {
               auto f = (Megalo::ReachForgeLabel*)(cobb::reference_tracked_object*)instance.relevant_objects[0];
               if (f->name) {
                  cobb::sprintf(out, "\"%s\"", f->name->english()); // TODO: this will break if the name contains double-quotes
                  return true;
               }
               cobb::sprintf(out, "%u", f->index);
               return true;
            }
            uint16_t index = instance.data.excerpt(1, index_bits);
            cobb::sprintf(out, "%u", index);
            return true;
         },
         nullptr // TODO: "compile" functor
      );
   }
}