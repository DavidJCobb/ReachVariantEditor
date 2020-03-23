#include "conditions.h"
#include "opcode_arg_types/all.h"
#include "../../errors.h"

#define MEGALO_DISALLOW_NONE_CONDITION 0
#if _DEBUG
   #undef  MEGALO_DISALLOW_NONE_CONDITION
   //#define MEGALO_DISALLOW_NONE_CONDITION 1
#endif

namespace Megalo {
   extern std::array<ConditionFunction, 18> conditionFunctionList = {{
      //
      // The double-curly-braces for this array are NOT a mistake; you should be able to 
      // use single braces but not every compiler handles that correctly.
      //
      ConditionFunction( // 0
         "None",
         "This condition does nothing.",
         "None.",
         {},
         OpcodeFuncToScriptMapping()
      ),
      ConditionFunction( // 1
         "Compare",
         "Compares any two values.",
         "%1 %v %3 %2.",
         {
            OpcodeArgBase("a", OpcodeArgAnyVariableFactory),
            OpcodeArgBase("b", OpcodeArgAnyVariableFactory),
            OpcodeArgBase("operator", OpcodeArgValueCompareOperatorEnum::factory)
         },
         OpcodeFuncToScriptMapping::make_intrinsic_comparison({0, 1}, 2)
      ),
      ConditionFunction( // 2
         "Object In Boundary",
         "Checks whether one object is inside of another object's Shape.",
         "%1 %v inside of %2's shape.",
         {
            OpcodeArgBase("a", OpcodeArgValueObject::typeinfo),
            OpcodeArgBase("b", OpcodeArgValueObject::typeinfo),
         },
         OpcodeFuncToScriptMapping::make_function("shape_contains", "", {0}, 1)
      ),
      ConditionFunction( // 3
         "Killer Type",
         "Checks what killed a player.",
         "%1 %v killed by any of: %2.",
         {
            OpcodeArgBase("victim", OpcodeArgValuePlayer::typeinfo),
            OpcodeArgBase("killer types", OpcodeArgValueKillerTypeFlags::factory),
         },
         OpcodeFuncToScriptMapping::make_function("killer_type_is", "", {1}, 0),
         "was", "was not"
      ),
      ConditionFunction( // 4
         "Team Disposition",
         "",
         "%1 %v disposition %3 with %2.",
         {
            OpcodeArgBase("a", OpcodeArgValueTeam::typeinfo),
            OpcodeArgBase("b", OpcodeArgValueTeam::typeinfo),
            OpcodeArgBase("disposition", OpcodeArgValueTeamDispositionEnum::factory),
         },
         OpcodeFuncToScriptMapping::make_function("has_disposition", "", {1, 2}, 0),
         "has", "does not have"
      ),
      ConditionFunction( // 5
         "Timer Is Zero",
         "Checks whether a timer is at zero.",
         "%1 %v at zero.",
         {
            OpcodeArgBase("timer", OpcodeArgValueTimer::typeinfo),
         },
         OpcodeFuncToScriptMapping::make_function("is_zero", "", {}, 0)
      ),
      ConditionFunction( // 6
         "Object Type",
         "Checks an object's type.",
         "%1 %v of type %2.",
         {
            OpcodeArgBase("object", OpcodeArgValueObject::typeinfo),
            OpcodeArgBase("type",   OpcodeArgValueMPObjectTypeIndex::factory),
         },
         OpcodeFuncToScriptMapping::make_function("is_of_type", "", {1}, 0)
      ),
      ConditionFunction( // 7
         "Team Has Players",
         "Checks whether a team has one or more players on it.",
         "%1 %v one or more players on it.",
         {
            OpcodeArgBase("team", OpcodeArgValueTeam::typeinfo),
         },
         OpcodeFuncToScriptMapping::make_function("has_any_players", "", {}, 0),
         "has", "does not have"
      ),
      ConditionFunction( // 8
         "Object Out Of Bounds",
         "Checks whether an object is out of bounds.",
         "%1 %v out of bounds.",
         {
            OpcodeArgBase("object", OpcodeArgValueObject::typeinfo),
         },
         OpcodeFuncToScriptMapping::make_function("is_out_of_bounds", "", {}, 0)
      ),
      ConditionFunction( // 9
         "Deprecated-09",
         "This condition always returns false.", // TODO: does inverting it change that?
         "Never. (Unused argument: %1)",
         {
            OpcodeArgBase("player", OpcodeArgValuePlayer::typeinfo),
         },
         OpcodeFuncToScriptMapping::make_function("deprecated_09", "", {}, 0)
      ),
      ConditionFunction( // 10
         "Player Assisted Kill Of",
         "Checks whether one player assisted in the slaying of another player. Note that players don't count as \"assisting\" themselves.",
         "%1 %v in the killing of %2.",
         {
            OpcodeArgBase("attacker", OpcodeArgValuePlayer::typeinfo),
            OpcodeArgBase("victim",   OpcodeArgValuePlayer::typeinfo),
         },
         OpcodeFuncToScriptMapping::make_function("assisted_kill_of", "", {1}, 0),
         "assisted", "did not assist"
      ),
      ConditionFunction( // 11
         "Object Has Label",
         "Checks whether an object has a Forge label.",
         "%1 %v label %2.",
         {
            OpcodeArgBase("object", OpcodeArgValueObject::typeinfo),
            OpcodeArgBase("label",  OpcodeArgValueForgeLabel::factory),
         },
         OpcodeFuncToScriptMapping::make_function("has_forge_label", "", {1}, 0),
         "has", "does not have"
      ),
      ConditionFunction( // 12
         "Player Is Not Respawning",
         "Checks whether a player is NOT waiting to respawn.", // TODO: includes loadout cam time?
         "%1 %v waiting to respawn.",
         {
            OpcodeArgBase("player", OpcodeArgValuePlayer::typeinfo),
         },
         OpcodeFuncToScriptMapping::make_function("is_not_respawning", "", {}, 0),
         "is not", "is"
      ),
      ConditionFunction( // 13
         "Equipment In Use",
         "",
         "%1 %v in use.",
         {
            OpcodeArgBase("object", OpcodeArgValueObject::typeinfo),
         },
         OpcodeFuncToScriptMapping::make_function("is_in_use", "", {}, 0)
      ),
      ConditionFunction( // 14
         "Species Is Spartan",
         "Checks whether a player is a Spartan.",
         "%1 %v a Spartan.",
         {
            OpcodeArgBase("player", OpcodeArgValuePlayer::typeinfo),
         },
         OpcodeFuncToScriptMapping::make_function("is_spartan", "", {}, 0)
      ),
      ConditionFunction( // 15
         "Species Is Elite",
         "Checks whether a player is an Elite.",
         "%1 %v an Elite.",
         {
            OpcodeArgBase("player", OpcodeArgValuePlayer::typeinfo),
         },
         OpcodeFuncToScriptMapping::make_function("is_elite", "is_sangheili", {}, 0)
      ),
      ConditionFunction( // 16
         "Species Is Monitor",
         "Checks whether a player is a Monitor.",
         "%1 %v a Monitor.",
         {
            OpcodeArgBase("player", OpcodeArgValuePlayer::typeinfo),
         },
         OpcodeFuncToScriptMapping::make_function("is_monitor", "", {}, 0)
      ),
      ConditionFunction( // 17
         "In Matchmaking",
         "This condition is believed to test whether the current match is taking place in Matchmaking.",
         "This match %v taking place in Matchmaking.",
         {},
         OpcodeFuncToScriptMapping::make_function("is_in_matchmaking", "", {}, OpcodeFuncToScriptMapping::game_namespace)
      ),
   }};

   bool Condition::read(cobb::ibitreader& stream) noexcept {
      #ifdef _DEBUG
         this->bit_offset = stream.get_bitpos();
      #endif
      {
         auto&  list  = conditionFunctionList;
         size_t index = stream.read_bits<size_t>(cobb::bitcount(list.size() - 1));
         if (index >= list.size()) {
            auto& error = GameEngineVariantLoadError::get();
            error.state         = GameEngineVariantLoadError::load_state::failure;
            error.failure_point = GameEngineVariantLoadError::load_failure_point::megalo_conditions;
            error.reason        = GameEngineVariantLoadError::load_failure_reason::invalid_script_opcode_function_index;
            error.extra[0]      = index;
            return false;
         }
         this->function = &list[index];
         if (index == 0) { // The "None" condition loads no further data.
            #if MEGALO_DISALLOW_NONE_CONDITION == 1
               auto& error = GameEngineVariantLoadError::get();
               error.state         = GameEngineVariantLoadError::load_state::failure;
               error.failure_point = GameEngineVariantLoadError::load_failure_point::megalo_conditions;
               error.reason        = GameEngineVariantLoadError::load_failure_reason::script_opcode_cannot_be_none;
               return false;
            #endif
            return true;
         }
      }
      stream.read(this->inverted);
      this->or_group.read(stream);
      this->action.read(stream);
      //
      auto&  base     = this->function->arguments;
      size_t argCount = base.size();
      this->arguments.resize(argCount);
      for (size_t i = 0; i < argCount; i++) {
         auto factory = base[i].factory;
         #pragma region HACKHACKHACK
            if (!factory && base[i].typeinfo)
               factory = base[i].typeinfo->factory;
         #pragma endregion
         this->arguments[i] = factory(stream);
         if (this->arguments[i]) {
            if (!this->arguments[i]->read(stream)) {
               auto& error = GameEngineVariantLoadError::get();
               error.state            = GameEngineVariantLoadError::load_state::failure;
               error.failure_point    = GameEngineVariantLoadError::load_failure_point::megalo_conditions;
               error.reason           = GameEngineVariantLoadError::load_failure_reason::bad_script_opcode_argument;
               error.failure_subindex = i; // (failure_index) must be set by the caller
               return false;
            }
         } else {
            auto& error = GameEngineVariantLoadError::get();
            error.state            = GameEngineVariantLoadError::load_state::failure;
            error.failure_point    = GameEngineVariantLoadError::load_failure_point::megalo_conditions;
            error.reason           = GameEngineVariantLoadError::load_failure_reason::bad_script_opcode_argument;
            if (!error.has_detail()) // some arg factory functions may provide specific failure details
               error.detail = GameEngineVariantLoadError::load_failure_detail::failed_to_construct_script_opcode_arg;
            error.failure_subindex = i; // (failure_index) must be set by the caller
            return false;
         }
      }
      #if _DEBUG
         this->to_string(this->debug_str);
      #endif
      return true;
   }
   void Condition::write(cobb::bitwriter& stream) const noexcept {
      #if _DEBUG
         printf("Saving condition: %s\n", this->debug_str.c_str());
      #endif
      {
         size_t index = 0;
         auto&  list   = conditionFunctionList;
         for (; index < list.size(); index++)
            if (this->function == &list[index])
               break;
         if (index == list.size()) {
            assert(false && "Condition with unknown function -- can't serialize!");
         }
         stream.write(index, cobb::bitcount(list.size() - 1));
         if (index == 0) // The "None" condition loads no further data.
            return;
      }
      stream.write(this->inverted);
      this->or_group.write(stream);
      this->action.write(stream);
      //
      auto&  base     = this->function->arguments;
      size_t argCount = base.size();
      assert(this->arguments.size() == argCount && "A condition didn't have enough arguments during save!");
      for (size_t i = 0; i < argCount; i++) {
         #if _DEBUG
            printf(" - Serializing argument #%d...\n", i);
         #endif
         auto* arg     = this->arguments[i];
         auto  factory = base[i].factory;
         #pragma region HACKHACKHACK
            if (!factory && base[i].typeinfo)
               factory = base[i].typeinfo->factory;
         #pragma endregion
         assert(arg && "A condition's argument is nullptr during save!");
         //
         // This is really ugly but I've sort of painted myself into a corner here... Some 
         // arguments can take multiple variable types, and currently the variable classes 
         // have no way of "knowing" that that's how they got here.
         //
         if (factory == OpcodeArgAnyVariableFactory) {
            #if _DEBUG
               printf("   Opcode base type is <any-variable>. Serializing variable type code...\n");
            #endif
            stream.write((uint8_t)arg->get_variable_type(), 3);
         }
         //
         // Now we can serialize the argument value.
         //
         #if _DEBUG
            printf("   About to write argument of type %s...\n", typeid(*arg).name());
         #endif
         arg->write(stream);
         #if _DEBUG
            printf("   Wrote argument.\n");
         #endif
      }
   }
   void Condition::to_string(std::string& out) const noexcept {
      if (!this->function) {
         out = "<NO FUNC>";
         return;
      }
      auto f = this->function->format;
      assert(f && "Cannot stringify a condition whose function has no format string!");
      out.clear();
      size_t l = strlen(f);
      for (size_t i = 0; i < l; i++) {
         unsigned char c = f[i];
         if (c != '%') {
            out += c;
            continue;
         }
         c = f[++i];
         if (c == '%') {
            out += '%';
            continue;
         }
         if (c == 'v') {
            out += (this->inverted) ? this->function->verb_invert : this->function->verb_normal;
            continue;
         }
         if (c >= '1' && c <= '9') {
            c -= '1';
            if (c >= this->arguments.size()) {
               out += '%';
               out += (c + '1');
               continue;
            }
            std::string temporary;
            this->arguments[c]->to_string(temporary);
            out += temporary;
         }
      }
   }
   void Condition::decompile(Decompiler& out) noexcept {
      if (this->inverted)
         out.write("not ");
      this->function->decompile(out, this->arguments);
   }
}