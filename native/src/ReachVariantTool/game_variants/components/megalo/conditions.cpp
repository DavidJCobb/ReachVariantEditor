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
            OpcodeArgBase("a", OpcodeArgValueAnyVariable::typeinfo),
            OpcodeArgBase("b", OpcodeArgValueAnyVariable::typeinfo),
            OpcodeArgBase("operator", OpcodeArgValueCompareOperatorEnum::typeinfo)
         },
         OpcodeFuncToScriptMapping::make_intrinsic_comparison({0, 1}, 2)
      ),
      ConditionFunction( // 2
         "Object In Boundary",
         "Checks whether one object is inside of another object's Shape. Note that this is a centerpoint check. If, for example, you check whether a shape contains a Warthog, the check will pass only when the Warthog's centerpoint is inside of the shape. If this check is run on an attached object, it will instead test the centerpoint of the parent object; for example, if A is attached to B, then A will only count as being in C's shape when B's centerpoint is in C's shape, regardless of the position of A's centerpoint.",
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
            OpcodeArgBase("killer types", OpcodeArgValueKillerTypeFlags::typeinfo),
         },
         OpcodeFuncToScriptMapping::make_function("killer_type_is", "", {1}, 0),
         "was", "was not"
      ),
      ConditionFunction( // 4
         "Team Alliance Status",
         "",
         "%1 %v alliance status %3 with %2.",
         {
            OpcodeArgBase("a", OpcodeArgValueTeam::typeinfo),
            OpcodeArgBase("b", OpcodeArgValueTeam::typeinfo),
            OpcodeArgBase("status", OpcodeArgValueTeamAllianceStatus::typeinfo),
         },
         OpcodeFuncToScriptMapping::make_function("has_alliance_status", "", {1, 2}, 0),
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
            OpcodeArgBase("type",   OpcodeArgValueObjectType::typeinfo),
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
         "Player Is Fireteam Leader",
         "This condition always returns false.",
         "%1 %v the leader of their fireteam.",
         {
            OpcodeArgBase("player", OpcodeArgValuePlayer::typeinfo),
         },
         OpcodeFuncToScriptMapping::make_function("is_fireteam_leader", "", {}, 0)
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
            OpcodeArgBase("label",  OpcodeArgValueForgeLabel::typeinfo),
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
         "In Forge",
         "This condition is believed to test whether the current match is taking place in Forge. However, there is no known way to get gametype scripts to actually run in Forge.",
         "This match %v taking place in Forge.",
         {},
         OpcodeFuncToScriptMapping::make_function("is_in_forge", "", {}, OpcodeFuncToScriptMapping::game_namespace)
      ),
   }};

   bool Condition::read(cobb::ibitreader& stream, GameVariantDataMultiplayer& mp) noexcept {
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
         auto  factory = base[i].typeinfo.factory;
         this->arguments[i] = factory();
         if (this->arguments[i]) {
            if (!this->arguments[i]->read(stream, mp)) {
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
         auto* arg = this->arguments[i];
         arg->write(stream);
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
            auto cnd = (const ConditionFunction*)this->function;
            out += (this->inverted) ? cnd->verb_invert : cnd->verb_normal;
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
   void Condition::reset() noexcept {
      Opcode::reset(); // call super
      #if _DEBUG
         this->debug_str.clear();
      #endif
   }
   Opcode* Condition::clone() const noexcept {
      auto cnd = (Condition*)Opcode::clone(); // call super
      cnd->inverted = this->inverted;
      cnd->action   = this->action;
      cnd->or_group = this->or_group;
      return cnd;
   }
}