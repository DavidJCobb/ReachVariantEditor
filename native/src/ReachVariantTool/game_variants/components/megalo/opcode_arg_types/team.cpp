#include "team.h"
#include "../../../errors.h"

namespace {
   enum class _scopes {
      global,
      player,
      object,
      team,
      player_owner_team,
      object_owner_team,
      //
      _count,
   };
}
namespace Megalo {
   /*virtual*/ bool OpcodeArgValueTeam::read(cobb::ibitreader& stream) noexcept /*override*/ {
      this->scope = stream.read_bits(cobb::bitcount((int)_scopes::_count - 1));
      int which_bits = 0;
      int index_bits = 0;
      const VariableScope* variable_scope = nullptr;
      const VariableScope* owner_team_of  = nullptr;
      switch ((_scopes)this->scope) {
         case _scopes::global:
            index_bits = MegaloVariableScopeTeam.which_bits();
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
         case _scopes::player_owner_team:
            owner_team_of = &MegaloVariableScopePlayer;
            break;
         case _scopes::object_owner_team:
            owner_team_of = &MegaloVariableScopeObject;
            break;
         default:
            {
               auto& error = GameEngineVariantLoadError::get();
               error.state    = GameEngineVariantLoadError::load_state::failure;
               error.reason   = GameEngineVariantLoadError::load_failure_reason::bad_script_opcode_argument;
               error.detail   = GameEngineVariantLoadError::load_failure_detail::bad_variable_subtype;
               error.extra[0] = (int32_t)variable_type::team;
               error.extra[1] = this->scope;
            }
            return false;
      }
      if (variable_scope) {
         this->which = stream.read_bits<uint16_t>(variable_scope->which_bits());
         this->index = stream.read_bits<uint16_t>(variable_scope->index_bits(variable_type::team));
         if (!variable_scope->is_valid_which(this->which)) {
            auto& error = GameEngineVariantLoadError::get();
            error.state    = GameEngineVariantLoadError::load_state::failure;
            error.reason   = GameEngineVariantLoadError::load_failure_reason::bad_script_opcode_argument;
            error.detail   = GameEngineVariantLoadError::load_failure_detail::bad_variable_scope;
            error.extra[0] = (int32_t)getScopeConstantForObject(*variable_scope);
            error.extra[1] = this->which;
            error.extra[2] = this->index;
            error.extra[3] = (int32_t)variable_type::team;
            return false;
         }
         return true;
      } else if (owner_team_of) {
         this->which = stream.read_bits<uint16_t>(owner_team_of->which_bits());
         return true;
      }
      if (which_bits)
         this->which = stream.read_bits<uint16_t>(which_bits);
      if (index_bits)
         this->index = stream.read_bits<uint16_t>(index_bits);
      return true;
   }
   void OpcodeArgValueTeam::write(cobb::bitwriter& stream) const noexcept {
      stream.write(this->scope, cobb::bitcount((int)_scopes::_count - 1));
      int which_bits = 0;
      int index_bits = 0;
      const VariableScope* variable_scope = nullptr;
      const VariableScope* owner_team_of  = nullptr;
      switch ((_scopes)this->scope) {
         case _scopes::global:
            index_bits = MegaloVariableScopeTeam.which_bits();
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
         case _scopes::player_owner_team:
            owner_team_of = &MegaloVariableScopePlayer;
            break;
         case _scopes::object_owner_team:
            owner_team_of = &MegaloVariableScopeObject;
            break;
      }
      if (variable_scope) {
         stream.write(this->which, variable_scope->which_bits());
         stream.write(this->index, variable_scope->index_bits(variable_type::team));
         return;
      } else if (owner_team_of) {
         stream.write(this->which, owner_team_of->which_bits());
         return;
      }
      if (which_bits)
         stream.write(this->which, which_bits);
      if (index_bits)
         stream.write(this->index, index_bits);
   }
   /*virtual*/ void OpcodeArgValueTeam::to_string(std::string& out) const noexcept /*override*/ {
      const char* which_scope = nullptr;
      switch ((_scopes)this->scope) {
         case _scopes::global:
            which_scope = megalo_teams[this->index];
            if (which_scope)
               out = which_scope;
            else
               cobb::sprintf(out, "INVALID_TEAM[%u]", this->index);
            return;
         case _scopes::player:
            which_scope = megalo_players[this->which];
            if (which_scope)
               cobb::sprintf(out, "%s.Team[%u]", which_scope, this->index);
            else
               cobb::sprintf(out, "INVALID_PLAYER[%u].Team[%u]", this->which, this->index);
            return;
         case _scopes::object:
            which_scope = megalo_objects[this->which];
            if (which_scope)
               cobb::sprintf(out, "%s.Team[%u]", which_scope, this->index);
            else
               cobb::sprintf(out, "INVALID_OBJECT[%u].Team[%u]", this->which, this->index);
            return;
         case _scopes::team:
            which_scope = megalo_teams[this->which];
            if (which_scope)
               cobb::sprintf(out, "%s.Team[%u]", which_scope, this->index);
            else
               cobb::sprintf(out, "INVALID_TEAM[%u].Team[%u]", this->which, this->index);
            return;
         case _scopes::player_owner_team:
            which_scope = megalo_players[this->which];
            if (which_scope)
               cobb::sprintf(out, "%s's team", which_scope);
            else
               cobb::sprintf(out, "INVALID_PLAYER[%u]'s team", this->which);
            return;
         case _scopes::object_owner_team:
            which_scope = megalo_objects[this->which];
            if (which_scope)
               cobb::sprintf(out, "%s's team", which_scope);
            else
               cobb::sprintf(out, "INVALID_OBJECT[%u]'s team", this->which);
            return;
      }
   }
}