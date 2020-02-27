#include "all_basic_flags.h"
#include "../../../helpers/strings.h"

namespace MegaloEx {
   namespace type_helpers {
      bool flags_arg_functor_load(const DetailedFlags& dfn, arg_functor_state fs, cobb::bitarray128& data, arg_rel_obj_list_t& relObjs, cobb::uint128_t input_bits) {
         data.consume(input_bits, dfn.bitcount());
         return true;
      }
      bool flags_arg_functor_to_english(const DetailedFlags& dfn, arg_functor_state fs, cobb::bitarray128& data, arg_rel_obj_list_t& relObjs, std::string& out) {
         out.clear();
         uint8_t  count = dfn.bitcount();
         uint32_t value = data.excerpt(fs.bit_offset, dfn.bitcount());
         uint8_t  found = 0;
         for (uint8_t i = 0; i < count; i++) {
            if (value & (1 << i)) {
               if (found > 1)
                  out += ", ";
               auto* entry = dfn.item(i);
               if (entry) {
                  QString n = entry->get_friendly_name();
                  if (n.isEmpty()) {
                     n = QString::fromLatin1(entry->name.c_str());
                     if (n.isEmpty())
                        n = QString("unnamed %1").arg(i);
                  }
                  out += n.toStdString();
               } else
                  cobb::sprintf(out, "%sinvalid %u", out.c_str(), i);
            }
         }
         if (!found)
            out = "none";
         return true;
      }
      bool flags_arg_functor_decompile(const DetailedFlags& dfn, arg_functor_state fs, cobb::bitarray128& data, arg_rel_obj_list_t& relObjs, std::string& out) {
         out.clear();
         uint8_t  count = dfn.bitcount();
         uint32_t value = data.excerpt(fs.bit_offset, dfn.bitcount());
         uint8_t  found = 0;
         for (uint8_t i = 0; i < count; i++) {
            if (value & (1 << i)) {
               if (++found == 1)
                  out = '(' + out;
               else if (found > 1)
                  out += ", ";
               auto* entry = dfn.item(i);
               if (entry) {
                  QString n = QString::fromLatin1(entry->name.c_str());
                  if (n.isEmpty())
                     cobb::sprintf(out, "%s%u", out.c_str(), i);
                  else
                     out += n.toStdString();
               } else
                  cobb::sprintf(out, "%s%u", out.c_str(), i);
            }
         }
         if (found)
            out += ')';
         else
            out = "none";
         return true;
      }
   }
   namespace types {
      namespace flags_definitions {
         auto create_object_flags = DetailedFlags({
            DetailedFlagsValue("never_garbage_collect"),
            DetailedFlagsValue("unk_1"),
            DetailedFlagsValue("unk_2"),
         });
         auto killer_type = DetailedFlags({
            DetailedFlagsValue("guardians", DetailedFlagsValueInfo::make_description("The cause of death for players who are killed by an autonomous map obstacle, or whose cause of death cannot be determined.")),
            DetailedFlagsValue("suicide",   DetailedFlagsValueInfo::make_description("The cause of death for players who die by their own hand.")),
            DetailedFlagsValue("kill",      DetailedFlagsValueInfo::make_description("The cause of death for players who are killed by an enemy player.")),
            DetailedFlagsValue("betrayal",  DetailedFlagsValueInfo::make_description("The cause of death for players who are killed by a teammate.")),
            DetailedFlagsValue("quit",      DetailedFlagsValueInfo::make_description("The cause of death for players who die as a result of quitting the match.")),
         });
         auto player_unused_mode = DetailedFlags({
            DetailedFlagsValue("unk_0"),
            DetailedFlagsValue("unk_1"),
            DetailedFlagsValue("unk_2"),
            DetailedFlagsValue("unk_3"),
         });
      }
      //
      #pragma region Typeinfo: create_object_flags
      OpcodeArgTypeinfo create_object_flags = OpcodeArgTypeinfo(
         QString("Create-Object Options"),
         QString("Options for creating objects."),
         OpcodeArgTypeinfo::flags::none,
         //
         [](arg_functor_state fs, cobb::bitarray128& data, arg_rel_obj_list_t& relObjs, cobb::uint128_t input_bits) { // loader
            return type_helpers::flags_arg_functor_load(flags_definitions::create_object_flags, fs, data, relObjs, input_bits);
         },
         OpcodeArgTypeinfo::default_postprocess_functor,
         [](arg_functor_state fs, cobb::bitarray128& data, arg_rel_obj_list_t& relObjs, std::string& out) { // to english
            return type_helpers::flags_arg_functor_to_english(flags_definitions::create_object_flags, fs, data, relObjs, out);
         },
         [](arg_functor_state fs, cobb::bitarray128& data, arg_rel_obj_list_t& relObjs, std::string& out) { // to script code
            return type_helpers::flags_arg_functor_decompile(flags_definitions::create_object_flags, fs, data, relObjs, out);
         },
         nullptr // TODO: "compile" functor
      );
      #pragma endregion
      #pragma region Typeinfo: killer_type
      OpcodeArgTypeinfo killer_type = OpcodeArgTypeinfo(
         QString("Killer Type"),
         QString("The kind of actor that caused a player's death."),
         OpcodeArgTypeinfo::flags::none,
         //
         [](arg_functor_state fs, cobb::bitarray128& data, arg_rel_obj_list_t& relObjs, cobb::uint128_t input_bits) { // loader
            return type_helpers::flags_arg_functor_load(flags_definitions::killer_type, fs, data, relObjs, input_bits);
         },
         OpcodeArgTypeinfo::default_postprocess_functor,
         [](arg_functor_state fs, cobb::bitarray128& data, arg_rel_obj_list_t& relObjs, std::string& out) { // to english
            return type_helpers::flags_arg_functor_to_english(flags_definitions::killer_type, fs, data, relObjs, out);
         },
         [](arg_functor_state fs, cobb::bitarray128& data, arg_rel_obj_list_t& relObjs, std::string& out) { // to script code
            return type_helpers::flags_arg_functor_decompile(flags_definitions::killer_type, fs, data, relObjs, out);
         },
         nullptr // TODO: "compile" functor
      );
      #pragma endregion
      #pragma region Typeinfo: player_unused_mode
      OpcodeArgTypeinfo player_unused_mode = OpcodeArgTypeinfo(
         QString("Unused Player Mode"),
         QString("Unknown."),
         OpcodeArgTypeinfo::flags::none,
         //
         [](arg_functor_state fs, cobb::bitarray128& data, arg_rel_obj_list_t& relObjs, cobb::uint128_t input_bits) { // loader
            return type_helpers::flags_arg_functor_load(flags_definitions::player_unused_mode, fs, data, relObjs, input_bits);
         },
         OpcodeArgTypeinfo::default_postprocess_functor,
         [](arg_functor_state fs, cobb::bitarray128& data, arg_rel_obj_list_t& relObjs, std::string& out) { // to english
            return type_helpers::flags_arg_functor_to_english(flags_definitions::player_unused_mode, fs, data, relObjs, out);
         },
         [](arg_functor_state fs, cobb::bitarray128& data, arg_rel_obj_list_t& relObjs, std::string& out) { // to script code
            return type_helpers::flags_arg_functor_decompile(flags_definitions::player_unused_mode, fs, data, relObjs, out);
         },
         nullptr // TODO: "compile" functor
      );
      #pragma endregion
   }
}