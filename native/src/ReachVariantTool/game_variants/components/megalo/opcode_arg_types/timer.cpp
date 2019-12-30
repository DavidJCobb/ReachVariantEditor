#include "timer.h"
#include "../../../errors.h"

namespace {
   enum class _scopes {
      global,
      player,
      team,
      object,
      round_time,
      sudden_death_time,
      grace_period_time,
      //
      _count,
   };
}
namespace Megalo {
   /*virtual*/ bool OpcodeArgValueTimer::read(cobb::bitreader& stream) noexcept /*override*/ {
      this->scope = stream.read_bits(cobb::bitcount((int)_scopes::_count - 1));
      int which_bits = 0;
      int index_bits = 0;
      const VariableScope* variable_scope = nullptr;
      switch ((_scopes)this->scope) {
         case _scopes::global:
            variable_scope = &MegaloVariableScopeGlobal;
            break;
         case _scopes::player:
            variable_scope = &MegaloVariableScopePlayer;
            break;
         case _scopes::object:
            variable_scope = &MegaloVariableScopeObject;
            break;
         case _scopes::team:
            variable_scope = &MegaloVariableScopeTeam;
            break;
         case _scopes::round_time:
         case _scopes::sudden_death_time:
         case _scopes::grace_period_time:
            break;
         default:
            {
               auto& error = GameEngineVariantLoadError::get();
               error.state    = GameEngineVariantLoadError::load_state::failure;
               error.reason   = GameEngineVariantLoadError::load_failure_reason::bad_script_opcode_argument;
               error.detail   = GameEngineVariantLoadError::load_failure_detail::bad_variable_subtype;
               error.extra[0] = (int32_t)variable_type::timer;
               error.extra[1] = this->scope;
            }
            return false;
      }
      if (variable_scope) {
         this->which = stream.read_bits<uint16_t>(variable_scope->which_bits());
         this->index = stream.read_bits<uint16_t>(variable_scope->index_bits(variable_type::timer));
         if (!variable_scope->is_valid_which(this->which)) {
            auto& error = GameEngineVariantLoadError::get();
            error.state    = GameEngineVariantLoadError::load_state::failure;
            error.reason   = GameEngineVariantLoadError::load_failure_reason::bad_script_opcode_argument;
            error.detail   = GameEngineVariantLoadError::load_failure_detail::bad_variable_scope;
            error.extra[0] = (int32_t)getScopeConstantForObject(*variable_scope);
            error.extra[1] = this->which;
            error.extra[2] = this->index;
            error.extra[3] = (int32_t)variable_type::timer;
            return false;
         }
         return true;
      }
      if (which_bits)
         this->which = stream.read_bits<uint16_t>(which_bits);
      if (index_bits)
         this->index = stream.read_bits<uint16_t>(index_bits);
      return true;
   }
   void OpcodeArgValueTimer::write(cobb::bitwriter& stream) const noexcept {
      stream.write(this->scope, cobb::bitcount((int)_scopes::_count - 1));
      int which_bits = 0;
      int index_bits = 0;
      const VariableScope* variable_scope = nullptr;
      switch ((_scopes)this->scope) {
         case _scopes::global:
            variable_scope = &MegaloVariableScopeGlobal;
            break;
         case _scopes::player:
            variable_scope = &MegaloVariableScopePlayer;
            break;
         case _scopes::object:
            variable_scope = &MegaloVariableScopeObject;
            break;
         case _scopes::team:
            variable_scope = &MegaloVariableScopeTeam;
            break;
         case _scopes::round_time:
         case _scopes::sudden_death_time:
         case _scopes::grace_period_time:
            break;
      }
      if (variable_scope) {
         stream.write(this->which, variable_scope->which_bits());
         stream.write(this->index, variable_scope->index_bits(variable_type::timer));
         return;
      }
      if (which_bits)
         stream.write(this->which, which_bits);
      if (index_bits)
         stream.write(this->index, index_bits);
   }
   /*virtual*/ void OpcodeArgValueTimer::to_string(std::string& out) const noexcept /*override*/ {
      const char* which_scope = nullptr;
      switch ((_scopes)this->scope) {
         case _scopes::global:
            cobb::sprintf(out, "Global.Timer[%d]", this->index);
            return;
         case _scopes::player:
            which_scope = megalo_players[this->which];
            if (which_scope)
               cobb::sprintf(out, "%s.Timer[%u]", which_scope, this->index);
            else
               cobb::sprintf(out, "INVALID_PLAYER[%u].Timer[%u]", this->which, this->index);
            return;
         case _scopes::object:
            which_scope = megalo_objects[this->which];
            if (which_scope)
               cobb::sprintf(out, "%s.Timer[%u]", which_scope, this->index);
            else
               cobb::sprintf(out, "INVALID_OBJECT[%u].Timer[%u]", this->which, this->index);
            return;
         case _scopes::team:
            which_scope = megalo_teams[this->which];
            if (which_scope)
               cobb::sprintf(out, "%s.Timer[%u]", which_scope, this->index);
            else
               cobb::sprintf(out, "INVALID_TEAM[%u].Timer[%u]", this->which, this->index);
            return;
         case _scopes::round_time:
            out = "Round Timer";
            return;
         case _scopes::sudden_death_time:
            out = "Sudden Death Timer";
            return;
         case _scopes::grace_period_time:
            out = "Grace Period Timer";
            return;
      }
   }
}