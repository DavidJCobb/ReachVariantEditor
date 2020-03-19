#include "player_or_group.h"
#include "../../../../errors.h"
#include "player.h"
#include "team.h"
#include "../../../megalo/variables_and_scopes.h"

namespace MegaloEx {
   namespace types {
      OpcodeArgTypeinfo player_or_group = OpcodeArgTypeinfo(
         QString("Player or Group"),
         QString("This type can refer to a player, a team, or to an \"all players\" sentinel value."),
         OpcodeArgTypeinfo::flags::may_need_postprocessing,
         //
         [](arg_functor_state fs, cobb::bitarray128& data, arg_rel_obj_list_t& relObjs, cobb::uint128_t input_bits) { // loader
            uint8_t type = (uint64_t)data.consume(input_bits, 2);
            //
            OpcodeArgTypeinfo* var_type = nullptr;
            switch (type) {
               case 0: var_type = &team;   break;
               case 1: var_type = &player; break;
               case 2:
                  return true;
            }
            if (var_type)
               return (var_type->load)(fs, data, relObjs, input_bits);
            //
            auto& error = GameEngineVariantLoadError::get();
            error.state    = GameEngineVariantLoadError::load_state::failure;
            error.reason   = GameEngineVariantLoadError::load_failure_reason::bad_script_opcode_argument;
            error.detail   = GameEngineVariantLoadError::load_failure_detail::bad_opcode_player_or_team_var_type;
            error.extra[0] = type;
            return false;
         },
         [](arg_functor_state fs, cobb::bitarray128& data, arg_rel_obj_list_t& relObjs, GameVariantData* variant) { // postprocess
            uint8_t type = (uint64_t)data.excerpt(fs.bit_offset, 2);
            fs.bit_offset += 2;
            //
            OpcodeArgTypeinfo* var_type = nullptr;
            switch (type) {
               case 0: var_type = &team;   break;
               case 1: var_type = &player; break;
               case 2:
                  return true;
            }
            if (var_type)
               return (var_type->postprocess)(fs, data, relObjs, variant);
            return false;
         },
         [](arg_functor_state fs, cobb::bitarray128& data, arg_rel_obj_list_t& relObjs, std::string& out) { // to english
            uint8_t type = (uint64_t)data.excerpt(fs.bit_offset, 2);
            fs.bit_offset += 2;
            //
            OpcodeArgTypeinfo* var_type = nullptr;
            switch (type) {
               case 0: var_type = &team;   break;
               case 1: var_type = &player; break;
               case 2:
                  out = "all players";
                  return true;
            }
            if (var_type)
               return (var_type->to_english)(fs, data, relObjs, out);
            return false;
         },
         [](arg_functor_state fs, cobb::bitarray128& data, arg_rel_obj_list_t& relObjs, std::string& out) { // to script code
            uint8_t type = (uint64_t)data.excerpt(fs.bit_offset, 2);
            fs.bit_offset += 2;
            //
            OpcodeArgTypeinfo* var_type = nullptr;
            switch (type) {
               case 0: var_type = &team;   break;
               case 1: var_type = &player; break;
               case 2:
                  out = "all_players";
                  return true;
            }
            if (var_type)
               return (var_type->decompile)(fs, data, relObjs, out);
            return false;
         },
         nullptr // TODO: "compile" functor
      );
   }
}