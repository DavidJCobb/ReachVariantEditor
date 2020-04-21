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
   bool VariableDeclaration::has_initial_value() const noexcept {
      switch (this->type) {
         case variable_type::scalar:
         case variable_type::team:
         case variable_type::timer:
            return true;
      }
      return false;
   }
   bool VariableDeclaration::has_network_type() const noexcept {
      switch (this->type) {
         case variable_type::scalar:
         case variable_type::player:
         case variable_type::object:
         case variable_type::team:
            return true;
      }
      return false;
   }
   void VariableDeclaration::read(cobb::ibitreader& stream, GameVariantDataMultiplayer& mp) noexcept {
      if (this->has_initial_value()) {
         if (this->type == variable_type::team)
            this->initial.team.read(stream);
         else {
            if (!this->initial.number)
               this->initial.number = new OpcodeArgValueScalar;
            this->initial.number->read(stream, mp);
         }
      }
      if (this->has_network_type())
         this->networking.read(stream);
   }
   void VariableDeclaration::write(cobb::bitwriter& stream) const noexcept {
      if (this->has_initial_value()) {
         if (this->type == variable_type::team)
            this->initial.team.write(stream);
         else {
            auto number = this->initial.number;
            bool temp   = false;
            if (!this->initial.number) {
               number = new OpcodeArgValueScalar;
               temp   = true;
            }
            number->write(stream);
            if (temp)
               delete number;
         }
      }
      if (this->has_network_type())
         this->networking.write(stream);
   }


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
         auto& decl    = this->scalars[i];
         auto  initial = decl.initial.number;
         if (decl.networking == variable_network_priority::default && initial->is_const_zero()) // treat zero as a default that can be omitted
            continue;
         out.write_line("declare ");
         out.write(_network_priority_to_string(decl.networking)); // includes space at the end
         out.write(scope);
         cobb::sprintf(temp, ".number[%u] = ", i);
         out.write(temp);
         initial->decompile(out, flags);
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
         auto&      decl = this->teams[i];
         const_team team = decl.initial.team;
         if (team == const_team::none && decl.networking == variable_network_priority::default)
            continue;
         out.write_line("declare ");
         out.write(_network_priority_to_string(decl.networking)); // includes space at the end
         out.write(scope);
         cobb::sprintf(temp, ".team[%u] = ", i);
         out.write(temp);
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
         if (decl.initial.number->is_const_zero()) // treat zero as a default that can be omitted
            continue;
         out.write_line("declare ");
         out.write(scope);
         cobb::sprintf(temp, ".timer[%u] = ", i);
         out.write(temp);
         decl.initial.number->decompile(out, flags);
      }
   }
   namespace {
      template<typename T> void _imply_impl(std::vector<T>& list, uint8_t index) {
         ++index;
         auto size = list.size();
         if (size >= index)
            return;
         list.resize(index);
         for (size_t i = size; i < index; ++i)
            list[i].compiler_flags |= VariableDeclaration::compile_flags::implicit;
      }
   }
   bool VariableDeclarationSet::imply(variable_type vt, uint8_t index) noexcept {
      auto& scope = getScopeObjectForConstant(this->type);
      auto  max   = scope.max_variables_of_type(vt);
      if (index >= max)
         return false;
      switch (vt) {
         case variable_type::scalar:
            _imply_impl(this->scalars, index);
            return true;
         case variable_type::object:
            _imply_impl(this->objects, index);
            return true;
         case variable_type::player:
            _imply_impl(this->players, index);
            return true;
         case variable_type::team:
            _imply_impl(this->teams, index);
            return true;
         case variable_type::timer:
            _imply_impl(this->timers, index);
            return true;
      }
      return true;
   }
   void VariableDeclarationSet::make_explicit(variable_type vt, uint8_t index) noexcept {
      auto& scope = getScopeObjectForConstant(this->type);
      auto  max   = scope.max_variables_of_type(vt);
      if (index >= max)
         return;
      switch (vt) {
         case variable_type::scalar:
            this->scalars[index].make_explicit();
            return;
         case variable_type::object:
            this->objects[index].make_explicit();
            return;
         case variable_type::player:
            this->players[index].make_explicit();
            return;
         case variable_type::team:
            this->teams[index].make_explicit();
            return;
         case variable_type::timer:
            this->timers[index].make_explicit();
            return;
      }
   }
   namespace {
      template<typename T> VariableDeclaration* _get_or_create_impl(std::vector<T>& list, uint8_t index) {
         auto size = list.size();
         if (index >= size) {
            list.resize(index + 1);
            for (size_t i = size; i <= index; ++i)
               list[i].compiler_flags |= VariableDeclaration::compile_flags::implicit;
         }
         return &list[index];
      }
   }
   VariableDeclaration* VariableDeclarationSet::get_or_create_declaration(variable_type vt, uint8_t index) noexcept {
      auto& scope = getScopeObjectForConstant(this->type);
      auto  max   = scope.max_variables_of_type(vt);
      if (index >= max)
         return nullptr;
      switch (vt) {
         case variable_type::scalar:
            return _get_or_create_impl(this->scalars, index);
         case variable_type::object:
            return _get_or_create_impl(this->objects, index);
         case variable_type::player:
            return _get_or_create_impl(this->players, index);
         case variable_type::team:
            return _get_or_create_impl(this->teams, index);
         case variable_type::timer:
            return _get_or_create_impl(this->timers, index);
      }
      return nullptr;
   }
}