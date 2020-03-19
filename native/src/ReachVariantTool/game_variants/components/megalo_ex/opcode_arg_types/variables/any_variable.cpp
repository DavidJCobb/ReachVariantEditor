#include "any_variable.h"
#include "../../../../errors.h"
#include "number.h"
#include "object.h"
#include "player.h"
#include "team.h"
#include "timer.h"
#include "../../../megalo/variables_and_scopes.h"

namespace MegaloEx {
   namespace types {
      OpcodeArgTypeinfo any_variable = OpcodeArgTypeinfo(
         QString("Any Variable"),
         QString(""),
         OpcodeArgTypeinfo::flags::may_need_postprocessing,
         //
         [](arg_functor_state fs, cobb::bitarray128& data, arg_rel_obj_list_t& relObjs, cobb::uint128_t input_bits) { // loader
            uint8_t type = (uint64_t)data.consume(input_bits, 3);
            //
            OpcodeArgTypeinfo* var_type = nullptr;
            switch ((Megalo::variable_type)type) {
               case Megalo::variable_type::scalar: var_type = &number; break;
               case Megalo::variable_type::timer:  var_type = &timer;  break;
               case Megalo::variable_type::team:   var_type = &team;   break;
               case Megalo::variable_type::player: var_type = &player; break;
               case Megalo::variable_type::object: var_type = &object; break;
            }
            if (var_type)
               return (var_type->load)(fs, data, relObjs, input_bits);
            //
            auto& error = GameEngineVariantLoadError::get();
            error.state    = GameEngineVariantLoadError::load_state::failure;
            error.reason   = GameEngineVariantLoadError::load_failure_reason::bad_script_opcode_argument;
            error.detail   = GameEngineVariantLoadError::load_failure_detail::bad_opcode_variable_type;
            error.extra[0] = type;
            return false;
         },
         [](arg_functor_state fs, cobb::bitarray128& data, arg_rel_obj_list_t& relObjs, GameVariantData* variant) { // postprocess
            uint8_t type = (uint64_t)data.excerpt(fs.bit_offset, 3);
            fs.bit_offset += 3;
            //
            OpcodeArgTypeinfo* var_type = nullptr;
            switch ((Megalo::variable_type)type) {
               case Megalo::variable_type::scalar: var_type = &number; break;
               case Megalo::variable_type::timer:  var_type = &timer;  break;
               case Megalo::variable_type::team:   var_type = &team;   break;
               case Megalo::variable_type::player: var_type = &player; break;
               case Megalo::variable_type::object: var_type = &object; break;
            }
            if (var_type)
               return (var_type->postprocess)(fs, data, relObjs, variant);
            return false;
         },
            [](arg_functor_state fs, cobb::bitarray128& data, arg_rel_obj_list_t& relObjs, std::string& out) { // to english
            uint8_t type = (uint64_t)data.excerpt(fs.bit_offset, 3);
            fs.bit_offset += 3;
            //
            OpcodeArgTypeinfo* var_type = nullptr;
            switch ((Megalo::variable_type)type) {
               case Megalo::variable_type::scalar: var_type = &number; break;
               case Megalo::variable_type::timer:  var_type = &timer;  break;
               case Megalo::variable_type::team:   var_type = &team;   break;
               case Megalo::variable_type::player: var_type = &player; break;
               case Megalo::variable_type::object: var_type = &object; break;
            }
            if (var_type)
               return (var_type->to_english)(fs, data, relObjs, out);
            return false;
         },
            [](arg_functor_state fs, cobb::bitarray128& data, arg_rel_obj_list_t& relObjs, std::string& out) { // to script code
            uint8_t type = (uint64_t)data.excerpt(fs.bit_offset, 3);
            fs.bit_offset += 3;
            //
            OpcodeArgTypeinfo* var_type = nullptr;
            switch ((Megalo::variable_type)type) {
               case Megalo::variable_type::scalar: var_type = &number; break;
               case Megalo::variable_type::timer:  var_type = &timer;  break;
               case Megalo::variable_type::team:   var_type = &team;   break;
               case Megalo::variable_type::player: var_type = &player; break;
               case Megalo::variable_type::object: var_type = &object; break;
            }
            if (var_type)
               return (var_type->decompile)(fs, data, relObjs, out);
            return false;
         },
         nullptr // TODO: "compile" functor
      );
   }
}