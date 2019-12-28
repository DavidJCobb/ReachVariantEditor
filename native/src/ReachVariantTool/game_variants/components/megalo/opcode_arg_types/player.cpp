#include "player.h"
#include "../parse_error_reporting.h"

namespace {
   enum class _scopes {
      global,
      player,
      object,
      team,
      //
      _count,
   };
}
namespace Megalo {
   /*virtual*/ bool OpcodeArgValuePlayer::read(cobb::bitreader& stream) noexcept /*override*/ {
      this->scope = stream.read_bits(cobb::bitcount((int)_scopes::_count - 1));
      int which_bits = 0;
      int index_bits = 0;
      const VariableScope* variable_scope = nullptr;
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
         default:
            {
               auto& error = ParseState::get();
               error.signalled = true;
               error.cause     = ParseState::what::bad_variable_subtype;
               error.extra[0]  = (uint32_t)variable_type::player;
               error.extra[1]  = this->scope;
            }
            return false;
      }
      if (variable_scope) {
         this->which = stream.read_bits<uint16_t>(variable_scope->which_bits());
         this->index = stream.read_bits<uint16_t>(variable_scope->index_bits(variable_type::player));
         return true;
      }
      if (which_bits)
         this->which = stream.read_bits<uint16_t>(which_bits);
      if (index_bits)
         this->index = stream.read_bits<uint16_t>(index_bits);
      return true;
   }
   void OpcodeArgValuePlayer::write(cobb::bitwriter& stream) const noexcept {
      stream.write(this->scope, cobb::bitcount((int)_scopes::_count - 1));
      int which_bits = 0;
      int index_bits = 0;
      const VariableScope* variable_scope = nullptr;
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
      }
      if (variable_scope) {
         stream.write(this->which, variable_scope->which_bits());
         stream.write(this->index, variable_scope->index_bits(variable_type::player));
         return;
      }
      if (which_bits)
         stream.write(this->which, which_bits);
      if (index_bits)
         stream.write(this->index, index_bits);
   }
   /*virtual*/ void OpcodeArgValuePlayer::to_string(std::string& out) const noexcept /*override*/ {
      const char* which_scope = nullptr;
      switch ((_scopes)this->scope) {
         case _scopes::global:
            which_scope = megalo_players[this->index];
            if (which_scope)
               out = which_scope;
            else
               cobb::sprintf(out, "INVALID_PLAYER[%u]", this->index);
            return;
         case _scopes::player:
            which_scope = megalo_players[this->which];
            if (which_scope)
               cobb::sprintf(out, "%s.Player[%u]", which_scope, this->index);
            else
               cobb::sprintf(out, "INVALID_PLAYER[%u].Player[%u]", this->which, this->index);
            return;
         case _scopes::object:
            which_scope = megalo_objects[this->which];
            if (which_scope)
               cobb::sprintf(out, "%s.Player[%u]", which_scope, this->index);
            else
               cobb::sprintf(out, "INVALID_OBJECT[%u].Player[%u]", this->which, this->index);
            return;
         case _scopes::team:
            which_scope = megalo_teams[this->which];
            if (which_scope)
               cobb::sprintf(out, "%s.Player[%u]", which_scope, this->index);
            else
               cobb::sprintf(out, "INVALID_TEAM[%u].Player[%u]", this->which, this->index);
            return;
      }
   }
}