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
      //  - This type stores a reference to one relevant-object.
      //
      OpcodeArgTypeinfo forge_label = OpcodeArgTypeinfo(
         QString("Forge Label"),
         QString(""),
         OpcodeArgTypeinfo::flags::may_need_postprocessing,
         //
         [](arg_functor_state fs, OpcodeArgValue& arg, cobb::uint128_t input_bits) { // loader
            auto& data = arg.data;
            auto& ro   = arg.relevant_objects;
            if (data.consume(input_bits, 1) == 0) { // absence bit; 0 means we have a value
               ro.ranges[fs.obj_index].start = data.size;
               ro.ranges[fs.obj_index].count = index_bits;
               data.consume(input_bits, index_bits);
            } else
               ro.ranges[fs.obj_index].count = 0;
            return true;
         },
         [](arg_functor_state fs, OpcodeArgValue& arg, GameVariantData* vd) { // postprocess
            auto& data = arg.data;
            auto& ro   = arg.relevant_objects;
            /*// Checking for absence this way would prevent us from ever handling this type if it were nested inside of another type.
            bool absence = data.size == fs.bit_offset + 1;
            if (absence)
               return true;
            //*/
            if (!ro.index_is_set(fs.obj_index)) {
               ro.pointers[fs.obj_index] = nullptr;
               return 1;
            }
            int32_t bitcount = 1 + ro.ranges[fs.obj_index].count;
            auto mp = dynamic_cast<GameVariantDataMultiplayer*>(vd);
            if (!mp)
               return OpcodeArgTypeinfo::functor_failed;
            uint16_t index = arg.excerpt_loaded_index(fs.obj_index);
            auto& list = mp->scriptContent.forgeLabels;
            if (list.size() <= index)
               return OpcodeArgTypeinfo::functor_failed;
            ro.pointers[fs.obj_index] = &list[index];
            return bitcount;
         },
         [](arg_functor_state fs, OpcodeArgValue& arg, std::string& out) { // to english
            auto& data = arg.data;
            auto& ro   = arg.relevant_objects;
            if (!ro.index_is_set(fs.obj_index)) {
               out = "no Forge label";
               return 1;
            }
            int32_t bitcount = 1 + ro.ranges[fs.obj_index].count;
            auto    obj      = ro.pointers[fs.obj_index].pointer_cast<Megalo::ReachForgeLabel>();
            if (obj) {
               if (obj->name) {
                  out = obj->name->english();
                  return bitcount;
               }
               cobb::sprintf(out, "unnamed Forge label %u", obj->index);
               return bitcount;
            }
            uint16_t index = arg.excerpt_loaded_index(fs.obj_index);
            cobb::sprintf(out, "missing Forge label %u", index);
            return bitcount;
         },
         [](arg_functor_state fs, OpcodeArgValue& arg, std::string& out) { // to script code
            auto& data = arg.data;
            auto& ro   = arg.relevant_objects;
            if (!ro.index_is_set(fs.obj_index)) {
               out = "none";
               return 1;
            }
            int32_t bitcount = 1 + ro.ranges[fs.obj_index].count;
            auto    obj      = ro.pointers[fs.obj_index].pointer_cast<Megalo::ReachForgeLabel>();
            if (obj) {
               if (obj->name) {
                  cobb::sprintf(out, "\"%s\"", obj->name->english()); // TODO: this will break if the name contains double-quotes
                  return bitcount;
               }
               cobb::sprintf(out, "%u", obj->index);
               return bitcount;
            }
            uint16_t index = arg.excerpt_loaded_index(fs.obj_index);
            cobb::sprintf(out, "%u", index);
            return bitcount;
         },
         nullptr // TODO: "compile" functor
      );
   }
}