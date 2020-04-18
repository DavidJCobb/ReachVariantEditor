#include "variable_declarations.h"

namespace {
   using namespace Megalo;
   const char* _network_priority_to_string(variable_network_priority p) {
      switch (p) {
         case variable_network_priority::none:
            return "local ";
         case variable_network_priority::default:
            return "default priority ";
         case variable_network_priority::low:
            return "low priority ";
         case variable_network_priority::high:
            return "high priority ";
      }
      return "";
   }
}
namespace Megalo {
   void VariableDeclarationSet::decompile(Decompiler& out, uint32_t flags) noexcept {
      std::string temp;
      std::string scope;
      switch (this->type) {
         case variable_scope::global: scope = "global"; break;
         case variable_scope::object: scope = "object"; break;
         case variable_scope::player: scope = "player"; break;
         case variable_scope::team:   scope = "team";   break;
      }
      for (size_t i = 0; i < this->scalars.size(); ++i) {
         auto& decl = this->scalars[i];
         if (decl.networking == variable_network_priority::default && decl.initial->is_const_zero()) // treat zero as a default that can be omitted
            continue;
         out.write_line("declare ");
         out.write(_network_priority_to_string(decl.networking)); // includes space at the end
         out.write(scope);
         cobb::sprintf(temp, ".number[%u] = ", i);
         out.write(temp);
         decl.initial->decompile(out, flags);
      }
      for (size_t i = 0; i < this->objects.size(); ++i) {
         auto& decl = this->objects[i];
         if (decl.networking == variable_network_priority::default)
            continue;
         out.write_line("declare ");
         out.write(_network_priority_to_string(decl.networking)); // includes space at the end
         out.write(scope);
         cobb::sprintf(temp, ".object[%u]", i);
         out.write(temp);
         //
         // Objects cannot have initial values.
         //
      }
      for (size_t i = 0; i < this->players.size(); ++i) {
         auto& decl = this->players[i];
         if (decl.networking == variable_network_priority::default)
            continue;
         out.write_line("declare ");
         out.write(_network_priority_to_string(decl.networking)); // includes space at the end
         out.write(scope);
         cobb::sprintf(temp, ".player[%u]", i);
         out.write(temp);
         //
         // Players cannot have initial values.
         //
      }
      for (size_t i = 0; i < this->teams.size(); ++i) {
         auto& decl = this->teams[i];
         if (decl.initial == const_team::none && decl.networking == variable_network_priority::default)
            continue;
         out.write_line("declare ");
         out.write(_network_priority_to_string(decl.networking)); // includes space at the end
         out.write(scope);
         cobb::sprintf(temp, ".team[%u] = ", i);
         out.write(temp);
         auto team = decl.initial;
         if (team >= const_team::team_1 && team <= const_team::team_8)
            cobb::sprintf(temp, "team[%u]", (int)team - (int)const_team::team_1);
         else if (team == const_team::neutral)
            temp = "neutral_team";
         else if (team == const_team::none)
            temp = "no_team";
         out.write(temp);
      }
      for (size_t i = 0; i < this->timers.size(); ++i) {
         //
         // Note: Timers do not have a networking priority.
         //
         auto& decl = this->timers[i];
         if (decl.initial->is_const_zero()) // treat zero as a default that can be omitted
            continue;
         out.write_line("declare ");
         out.write(scope);
         cobb::sprintf(temp, ".timer[%u] = ", i);
         out.write(temp);
         decl.initial->decompile(out, flags);
      }
   }
}