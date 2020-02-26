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
         [](fragment_specifier fs, cobb::bitarray128& data, arg_rel_obj_list_t& relObjs, cobb::uint128_t input_bits) { // loader
            if (data.consume(input_bits, 1) == 0) { // absence bit; 0 means we have a value
               relObjs.ranges[fs.obj_index].start = data.size;
               relObjs.ranges[fs.obj_index].count = index_bits;
               data.consume(input_bits, index_bits);
            } else
               relObjs.ranges[fs.obj_index].count = 0;
            return true;
         },
         [](fragment_specifier fs, cobb::bitarray128& data, arg_rel_obj_list_t& relObjs, GameVariantData* vd) { // postprocess
            /*// Checking for absence this way would prevent us from ever handling this type if it were nested inside of another type.
            bool absence = data.size == fs.bit_offset + 1;
            if (absence)
               return true;
            //*/
            if (!relObjs.index_is_set(fs.obj_index)) {
               relObjs.pointers[fs.obj_index] = nullptr;
               return true;
            }
            auto mp = dynamic_cast<GameVariantDataMultiplayer*>(vd);
            if (!mp)
               return false;
            uint16_t index = OpcodeArgValue::excerpt_loaded_index(data, relObjs, fs.obj_index);
            auto& list = mp->scriptContent.forgeLabels;
            if (list.size() <= index)
               return false;
            relObjs.pointers[fs.obj_index] = &list[index];
            return true;
         },
         [](fragment_specifier fs, cobb::bitarray128& data, arg_rel_obj_list_t& relObjs, std::string& out) { // to english
            if (!relObjs.index_is_set(fs.obj_index)) {
               out = "no Forge label";
               return true;
            }
            auto obj = relObjs.pointers[fs.obj_index].pointer_cast<Megalo::ReachForgeLabel>();
            if (obj) {
               if (obj->name) {
                  out = obj->name->english();
                  return true;
               }
               cobb::sprintf(out, "unnamed Forge label %u", obj->index);
               return true;
            }
            uint16_t index = OpcodeArgValue::excerpt_loaded_index(data, relObjs, fs.obj_index);
            cobb::sprintf(out, "missing Forge label %u", index);
            return true;
         },
         [](fragment_specifier fs, cobb::bitarray128& data, arg_rel_obj_list_t& relObjs, std::string& out) { // to script code
            if (!relObjs.index_is_set(fs.obj_index)) {
               out = "none";
               return true;
            }
            auto obj = relObjs.pointers[fs.obj_index].pointer_cast<Megalo::ReachForgeLabel>();
            if (obj) {
               if (obj->name) {
                  cobb::sprintf(out, "\"%s\"", obj->name->english()); // TODO: this will break if the name contains double-quotes
                  return true;
               }
               cobb::sprintf(out, "%u", obj->index);
               return true;
            }
            uint16_t index = OpcodeArgValue::excerpt_loaded_index(data, relObjs, fs.obj_index);
            cobb::sprintf(out, "%u", index);
            return true;
         },
         nullptr // TODO: "compile" functor
      );
   }
}