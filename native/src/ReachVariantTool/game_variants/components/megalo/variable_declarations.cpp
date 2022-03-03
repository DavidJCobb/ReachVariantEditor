#include "variable_declarations.h"

namespace {
   using namespace Megalo;
   const char* _network_priority_to_string(variable_network_priority p) {
      switch (p) {
         case variable_network_priority::none:
            return "with network priority local";
         case variable_network_priority::low:
            return "with network priority low";
         case variable_network_priority::high:
            return "with network priority high";
      }
      return "";
   }
}
namespace Megalo {
   extern constexpr bool variable_type_can_have_initializer(variable_type v) {
      switch (v) {
         case variable_type::scalar:
         case variable_type::team:
         case variable_type::timer:
            return true;
      }
      return false;
   }
   extern constexpr bool variable_type_has_network_priority(variable_type v) {
      switch (v) {
         case variable_type::scalar:
         case variable_type::player:
         case variable_type::object:
         case variable_type::team:
            return true;
      }
      return false;
   }

   VariableDeclaration::VariableDeclaration(variable_type vt) : type(vt) {
      if (this->has_initial_value()) {
         if (this->type != variable_type::team) {
            this->initial.number = new OpcodeArgValueScalar;
            this->initial.number->set_to_const_zero();
         }
      }
   }
   VariableDeclaration::~VariableDeclaration() {
      if (this->initial.number) {
         delete this->initial.number;
         this->initial.number = nullptr;
      }
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
   void VariableDeclaration::decompile(const char* scope_name, uint8_t var_index, Decompiler& out, uint32_t flags) noexcept {
      std::string name;
      switch (this->type) {
         case variable_type::scalar: name = ".number[%u]"; break;
         case variable_type::object: name = ".object[%u]"; break;
         case variable_type::player: name = ".player[%u]"; break;
         case variable_type::team:   name = ".team[%u]";   break;
         case variable_type::timer:  name = ".timer[%u]";   break;
      }
      bool n_default = !this->has_network_type();
      bool i_default = this->initial_value_is_default();
      if (n_default && i_default)
         return;
      out.write_line("declare ");
      out.write(scope_name);
      cobb::sprintf(name, name.c_str(), var_index);
      out.write(name);
      if (!n_default) {
         out.write(' ');
         out.write(_network_priority_to_string(this->networking));
      }
      if (!i_default) {
         out.write(" = ");
         if (this->type == variable_type::team) {
            auto team = this->initial.team;
            std::string temp;
            if (team >= const_team::team_1 && team <= const_team::team_8)
               cobb::sprintf(temp, "team[%u]", (int)team - (int)const_team::team_1);
            else if (team == const_team::neutral)
               temp = "neutral_team";
            else if (team == const_team::none)
               temp = "no_team";
            out.write(temp);
         } else {
            this->initial.number->decompile(out, flags);
         }
      }
   }
   bool VariableDeclaration::initial_value_is_default() const noexcept {
      if (!this->has_initial_value())
         return true;
      if (this->type == variable_type::team)
         return this->initial.team == const_team::none;
      auto initial = this->initial.number;
      if (!initial)
         return true;
      return initial->is_const_zero();
   }

   void VariableDeclarationList::clear() noexcept {
      for (auto* decl : this->list)
         delete decl;
      this->list.clear();
   }
   void VariableDeclarationList::adopt(VariableDeclarationList& other) noexcept {
      this->clear();
      this->list.reserve(other.size());
      for (auto* decl : other.list)
         this->list.push_back(decl);
      other.list.clear(); // NOT other.clear, which would delete the list items
   }


   VariableDeclarationList& VariableDeclarationSet::variables_by_type(variable_type t) {
      switch (t) {
         case variable_type::scalar: return this->scalars;
         case variable_type::player: return this->players;
         case variable_type::object: return this->objects;
         case variable_type::team:   return this->teams;
         case variable_type::timer:  return this->timers;
      }
      __assume(0); // unreachable
   }
   const VariableDeclarationList& VariableDeclarationSet::variables_by_type(variable_type t) const {
      switch (t) {
         case variable_type::scalar: return this->scalars;
         case variable_type::player: return this->players;
         case variable_type::object: return this->objects;
         case variable_type::team:   return this->teams;
         case variable_type::timer:  return this->timers;
      }
      __assume(0); // unreachable
   }

   void VariableDeclarationSet::adopt(VariableDeclarationSet& other) noexcept {
      this->scalars.adopt(other.scalars);
      this->objects.adopt(other.objects);
      this->players.adopt(other.players);
      this->teams.adopt(other.teams);
      this->timers.adopt(other.timers);
   }
   void VariableDeclarationSet::decompile(Decompiler& out, uint32_t flags) noexcept {
      const char* scope = "SCOPE?";
      switch (this->type) {
         case variable_scope::global: scope = "global"; break;
         case variable_scope::object: scope = "object"; break;
         case variable_scope::player: scope = "player"; break;
         case variable_scope::team:   scope = "team";   break;
      }
      for (size_t i = 0; i < this->scalars.size(); ++i)
         this->scalars[i]->decompile(scope, i, out, flags);
      for (size_t i = 0; i < this->objects.size(); ++i)
         this->objects[i]->decompile(scope, i, out, flags);
      for (size_t i = 0; i < this->players.size(); ++i)
         this->players[i]->decompile(scope, i, out, flags);
      for (size_t i = 0; i < this->teams.size(); ++i)
         this->teams[i]->decompile(scope, i, out, flags);
      for (size_t i = 0; i < this->timers.size(); ++i)
         this->timers[i]->decompile(scope, i, out, flags);
   }
   namespace {
      void _imply_impl(VariableDeclarationList& list, uint8_t index) {
         ++index;
         auto size = list.size();
         if (size >= index)
            return;
         list.resize(index);
         for (size_t i = size; i < index; ++i)
            list[i]->compiler_flags |= VariableDeclaration::compile_flags::implicit;
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
            this->scalars[index]->make_explicit();
            return;
         case variable_type::object:
            this->objects[index]->make_explicit();
            return;
         case variable_type::player:
            this->players[index]->make_explicit();
            return;
         case variable_type::team:
            this->teams[index]->make_explicit();
            return;
         case variable_type::timer:
            this->timers[index]->make_explicit();
            return;
      }
   }
   namespace {
      VariableDeclaration* _get_or_create_impl(VariableDeclarationList& list, uint8_t index) {
         auto size = list.size();
         if (index >= size) {
            list.resize(index + 1);
            for (size_t i = size; i <= index; ++i)
               list[i]->compiler_flags |= VariableDeclaration::compile_flags::implicit;
         }
         return list[index];
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

   size_t VariableDeclarationSet::post_read_fixup() {
      size_t bad = 0;
      //
      auto fixup = [&bad, this](variable_type t) {
         if (!variable_type_has_network_priority(t))
            return;
         auto& list = this->variables_by_type(t);
         for (auto& item : list) {
            if ((int)item->networking == 3) {
               ++bad;
               item->networking = variable_network_priority::low;
            }
         }
      };
      fixup(variable_type::scalar);
      fixup(variable_type::object);
      fixup(variable_type::player);
      fixup(variable_type::team);
      fixup(variable_type::timer);
      return bad;
   }
}