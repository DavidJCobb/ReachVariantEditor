#include "object.h"

namespace {
   enum class _scopes {
      global,
      player,
      object,
      team,
      player_slave,
      player_player_slave,
      object_player_slave,
      team_player_slave,
      //
      _count,
   };
   //
   // From looking at CTF trigger actions, I think the "slave" object of a player is 
   // basically the "third weapon" i.e. a carried flag, bomb, or oddball.
   //
}
namespace Megalo {
   /*virtual*/ bool OpcodeArgValueObject::read(cobb::bitstream& stream) noexcept /*override*/ {
      this->scope = stream.read_bits(cobb::bitcount((int)_scopes::_count - 1));
      int which_bits = 0;
      int index_bits = 0;
      const VariableScope* variable_scope  = nullptr;
      const VariableScope* slave_var_scope = nullptr;
      switch ((_scopes)this->scope) {
         case _scopes::global:
            index_bits = MegaloVariableScopePlayer.which_bits();
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
         case _scopes::player_slave:
            which_bits = MegaloVariableScopePlayer.which_bits();
            break;
         case _scopes::player_player_slave:
            slave_var_scope = &MegaloVariableScopePlayer;
            break;
         case _scopes::object_player_slave:
            slave_var_scope = &MegaloVariableScopeObject;
            break;
         case _scopes::team_player_slave:
            slave_var_scope = &MegaloVariableScopeTeam;
            break;
         default:
            assert(false && "Unrecognized object subtype!");
            return false;
      }
      if (variable_scope) {
         this->which = stream.read_bits<uint16_t>(variable_scope->which_bits());
         this->index = stream.read_bits<uint16_t>(variable_scope->index_bits(variable_type::object));
         return true;
      } else if (slave_var_scope) {
         this->which = stream.read_bits<uint16_t>(slave_var_scope->which_bits());
         this->index = stream.read_bits<uint16_t>(slave_var_scope->index_bits(variable_type::player));
         return true;
      }
      if (which_bits)
         this->which = stream.read_bits<uint16_t>(which_bits);
      if (index_bits)
         this->index = stream.read_bits<uint16_t>(index_bits);
      return true;
   }
   /*virtual*/ void OpcodeArgValueObject::to_string(std::string& out) const noexcept /*override*/ {
      const char* which_scope = nullptr;
      switch ((_scopes)this->scope) {
         case _scopes::global:
            which_scope = megalo_objects[this->index];
            if (which_scope)
               out = which_scope;
            else
               cobb::sprintf(out, "INVALID_OBJECT[%u]", this->index);
            return;
         case _scopes::player:
            which_scope = megalo_players[this->which];
            if (which_scope)
               cobb::sprintf(out, "%s.Object[%u]", which_scope, this->index);
            else
               cobb::sprintf(out, "INVALID_PLAYER[%u].Object[%u]", this->which, this->index);
            return;
         case _scopes::object:
            which_scope = megalo_objects[this->which];
            if (which_scope)
               cobb::sprintf(out, "%s.Object[%u]", which_scope, this->index);
            else
               cobb::sprintf(out, "INVALID_OBJECT[%u].Object[%u]", this->which, this->index);
            return;
         case _scopes::team:
            which_scope = megalo_teams[this->which];
            if (which_scope)
               cobb::sprintf(out, "%s.Object[%u]", which_scope, this->index);
            else
               cobb::sprintf(out, "INVALID_TEAM[%u].Object[%u]", this->which, this->index);
            return;
         case _scopes::player_slave:
            which_scope = megalo_players[this->which];
            if (which_scope)
               cobb::sprintf(out, "%s.Slave", which_scope);
            else
               cobb::sprintf(out, "INVALID_PLAYER[%u].Slave", this->which);
            return;
         case _scopes::player_player_slave:
            which_scope = megalo_players[this->which];
            if (which_scope)
               cobb::sprintf(out, "%s.Player[%u].Slave", which_scope, this->index);
            else
               cobb::sprintf(out, "INVALID_PLAYER[%u].Player[%u].Slave", this->which, this->index);
            return;
         case _scopes::object_player_slave:
            which_scope = megalo_objects[this->which];
            if (which_scope)
               cobb::sprintf(out, "%s.Player[%u].Slave", which_scope, this->index);
            else
               cobb::sprintf(out, "INVALID_OBJECT[%u].Player[%u].Slave", this->which, this->index);
            return;
         case _scopes::team_player_slave:
            which_scope = megalo_teams[this->which];
            if (which_scope)
               cobb::sprintf(out, "%s.Player[%u].Slave", which_scope, this->index);
            else
               cobb::sprintf(out, "INVALID_TEAM[%u].Player[%u].Slave", this->which, this->index);
            return;
      }
   }
}