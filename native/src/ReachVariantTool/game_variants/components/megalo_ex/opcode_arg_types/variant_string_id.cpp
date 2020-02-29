#include "all_basic_enums.h"
#include "../../../helpers/strings.h"

namespace MegaloEx {
   namespace types {
      namespace enums {
         auto variant_string_id = DetailedEnum({
            DetailedEnumValue("mp_boneyard_a_idle_start"),
            DetailedEnumValue("mp_boneyard_a_fly_in"),
            DetailedEnumValue("mp_boneyard_a_idle_mid"),
            DetailedEnumValue("mp_boneyard_a_fly_out"),
            DetailedEnumValue("mp_boneyard_b_fly_in"),
            DetailedEnumValue("mp_boneyard_b_idle_mid"),
            DetailedEnumValue("mp_boneyard_b_fly_out"),
            DetailedEnumValue("mp_boneyard_b_idle_start"),
            DetailedEnumValue("mp_boneyard_a_leave1"),
            DetailedEnumValue("mp_boneyard_b_leave1"),
            DetailedEnumValue("mp_boneyard_b_pickup"),
            DetailedEnumValue("mp_boneyard_b_idle_pickup"),
            DetailedEnumValue("mp_boneyard_a"),
            DetailedEnumValue("mp_boneyard_b"),
            DetailedEnumValue("default"),
            DetailedEnumValue("carter"),
            DetailedEnumValue("jun"),
            DetailedEnumValue("female"),
            DetailedEnumValue("male"),
            DetailedEnumValue("emile"),
            DetailedEnumValue("player_skull"),
            DetailedEnumValue("kat"),
            DetailedEnumValue("minor"),
            DetailedEnumValue("officer"),
            DetailedEnumValue("ultra"),
            DetailedEnumValue("space"),
            DetailedEnumValue("spec_ops"),
            DetailedEnumValue("general"),
            DetailedEnumValue("zealot"),
            DetailedEnumValue("mp"),
            DetailedEnumValue("jetpack"),
            DetailedEnumValue("gauss"),
            DetailedEnumValue("troop"),
            DetailedEnumValue("rocket"),
            DetailedEnumValue("fr"), // frag?!
            DetailedEnumValue("pl"), // plasma?!
            DetailedEnumValue("35_spire_fp"),
            DetailedEnumValue("mp_spire_fp"),
         });
      }
      //
      // This type is basically an enum, but it allows for up to 256 values rather than being constrained to the actual number of 
      // defined entries in the enum.
      //
      OpcodeArgTypeinfo variant_string_id = OpcodeArgTypeinfo(
         QString("Variant String ID"),
         QString("Used to indicate that a \"Create Object\" instruction should spawn variants of existing objects, e.g. Spartan bipeds with specific predefined armor permutations."),
         OpcodeArgTypeinfo::flags::none,
         //
         [](arg_functor_state fs, cobb::bitarray128& data, arg_rel_obj_list_t& relObjs, cobb::uint128_t input_bits) { // loader
            data.consume(input_bits, cobb::bitcount(Megalo::Limits::max_string_ids - 1));
            return true;
         },
         OpcodeArgTypeinfo::default_postprocess_functor,
         [](arg_functor_state fs, cobb::bitarray128& data, arg_rel_obj_list_t& relObjs, std::string& out) { // to english
            auto e = enums::variant_string_id;
            //
            // Can't use the normal "enum helper" functions because this enum has an offset, i.e. a value 
            // of -1 is used to indicate "no sound" and to avoid sign bit mishaps, the game adds 1 before 
            // serializing and subtracts 1 when loading.
            //
            int32_t index = data.excerpt(fs.bit_offset, cobb::bitcount(Megalo::Limits::max_string_ids - 1));
            --index;
            if (index < 0) {
               out = "none";
               return true;
            }
            auto item = e.item(index);
            if (!item) {
               cobb::sprintf(out, "invalid value %u", index);
               return true;
            }
            QString name = item->get_friendly_name();
            if (!name.isEmpty()) {
               out = name.toStdString();
               return true;
            }
            out = item->name;
            if (out.empty()) // enum values should never be nameless but just in case
               cobb::sprintf(out, "%u", index);
            return true;
         },
         [](arg_functor_state fs, cobb::bitarray128& data, arg_rel_obj_list_t& relObjs, std::string& out) { // to script code
            auto e = enums::variant_string_id;
            //
            // Can't use the normal "enum helper" functions because this enum has an offset, i.e. a value 
            // of -1 is used to indicate "no sound" and to avoid sign bit mishaps, the game adds 1 before 
            // serializing and subtracts 1 when loading.
            //
            int32_t index = data.excerpt(fs.bit_offset, cobb::bitcount(Megalo::Limits::max_string_ids - 1));
            --index;
            if (index < 0) {
               out = "none";
               return true;
            }
            auto item = e.item(index);
            if (!item) {
               cobb::sprintf(out, "%u", index);
               return true;
            }
            out = item->name;
            if (out.empty()) // enum values should never be nameless but just in case
               cobb::sprintf(out, "%u", index);
            return true;
         },
         nullptr // TODO: "compile" functor
      );
   }
}