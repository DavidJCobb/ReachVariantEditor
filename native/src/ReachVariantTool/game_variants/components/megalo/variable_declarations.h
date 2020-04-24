#pragma once
#include "limits.h"
#include "variables_and_scopes.h"
#include "opcode_arg_types/all_enums.h"
#include "opcode_arg_types/variables/number.h"
#include "../../../helpers/bitnumber.h"
#include "../../../helpers/stream.h"
#include "../../../helpers/bitwriter.h"

namespace Megalo {
   enum class variable_network_priority {
      none, // offline
      low,
      high,
      default,
   };
   using variable_network_priority_t = cobb::bitnumber<2, variable_network_priority>;
   //
   class VariableDeclaration {
      public:
         using network_enum   = variable_network_priority;
         using network_type   = variable_network_priority_t;
         using team_bitnumber = cobb::bitnumber<cobb::bitcount(8), const_team, true>;
         //
         struct compile_flags {
            enum type : uint8_t {
               none = 0,
               implicit         = 0x01,
               implicit_network = 0x02,
               implicit_initial = 0x04,
            };
         };
         using compile_flags_t = std::underlying_type_t<compile_flags::type>;
         //
      protected:
         variable_type type = variable_type::not_a_variable;
         //
      public:
         VariableDeclaration(variable_type vt);
         ~VariableDeclaration();
         //
         compile_flags_t compiler_flags = compile_flags::none;
         network_type    networking     = network_enum::default;
         struct {
            OpcodeArgValueScalar* number = nullptr;
            team_bitnumber        team   = const_team::none;
         } initial;
         //
         [[nodiscard]] inline variable_type get_type() const noexcept { return this->type; }
         [[nodiscard]] inline bool is_implicit() const noexcept { return this->compiler_flags & compile_flags::implicit; }
         inline void make_explicit() noexcept { this->compiler_flags &= ~compile_flags::implicit; }
         [[nodiscard]] inline bool networking_is_implicit() const noexcept { return this->compiler_flags & compile_flags::implicit_network; }
         inline void make_networking_explicit() noexcept { this->compiler_flags &= ~compile_flags::implicit_network; }
         [[nodiscard]] inline bool initial_value_is_implicit() const noexcept { return this->compiler_flags & compile_flags::implicit_initial; }
         inline void make_initial_value_explicit() noexcept { this->compiler_flags &= ~compile_flags::implicit_initial; }
         //
         [[nodiscard]] bool has_initial_value() const noexcept;
         [[nodiscard]] bool has_network_type() const noexcept;
         void read(cobb::ibitreader& stream, GameVariantDataMultiplayer& mp) noexcept;
         void write(cobb::bitwriter& stream) const noexcept;
         void decompile(const char* scope_name, uint8_t var_index, Decompiler& out, uint32_t flags = 0) noexcept;
         //
         bool initial_value_is_default() const noexcept;
         bool network_type_is_default() const noexcept;
   };
   class ScalarVariableDeclaration : public VariableDeclaration {
      public:
         ScalarVariableDeclaration() : VariableDeclaration(variable_type::scalar) {}
   };
   class PlayerVariableDeclaration : public VariableDeclaration {
      public:
         PlayerVariableDeclaration() : VariableDeclaration(variable_type::player) {}
   };
   class ObjectVariableDeclaration : public VariableDeclaration {
      public:
         ObjectVariableDeclaration() : VariableDeclaration(variable_type::object) {}
   };
   class TeamVariableDeclaration : public VariableDeclaration {
      public:
         TeamVariableDeclaration() : VariableDeclaration(variable_type::team) {}
   };
   class TimerVariableDeclaration : public VariableDeclaration {
      public:
         TimerVariableDeclaration() : VariableDeclaration(variable_type::timer) {}
   };

   struct VariableDeclarationList {
      private:
         variable_type type;
         std::vector<VariableDeclaration*> list;
         //
         using iterator       = decltype(list)::iterator;
         using const_iterator = decltype(list)::const_iterator;
         //
      public:
         VariableDeclarationList(variable_type vt) : type(vt) {}
         ~VariableDeclarationList() {
            this->clear();
         }
         //
         inline VariableDeclaration* operator[](int i) const noexcept { return this->list[i]; }
         //
         inline iterator begin() noexcept { return this->list.begin(); }
         inline const_iterator begin() const noexcept { return this->list.begin(); }
         inline iterator end() noexcept { return this->list.end(); }
         inline const_iterator end() const noexcept { return this->list.end(); }
         inline size_t size() const noexcept { return this->list.size(); }
         //
         void resize(size_t size) noexcept {
            size_t prior = this->list.size();
            if (size == prior)
               return;
            this->list.resize(size);
            if (size < prior)
               return;
            for (; prior < size; ++prior)
               this->list[prior] = new VariableDeclaration(this->type);
         }
         //
         void clear() noexcept;
         void adopt(VariableDeclarationList& other) noexcept;
   };

   class VariableDeclarationSet {
      public:
         variable_scope type;
         VariableDeclarationList scalars = VariableDeclarationList(variable_type::scalar);
         VariableDeclarationList players = VariableDeclarationList(variable_type::player);
         VariableDeclarationList objects = VariableDeclarationList(variable_type::object);
         VariableDeclarationList teams   = VariableDeclarationList(variable_type::team);
         VariableDeclarationList timers  = VariableDeclarationList(variable_type::timer);
         //
         VariableDeclarationSet(variable_scope t) : type(t) {}
         //
         void read(cobb::ibitreader& stream, GameVariantDataMultiplayer& mp) noexcept {
            auto& scope = getScopeObjectForConstant(this->type);
            //
            #define megalo_variable_declaration_set_read_type(name) \
               this->##name##s.resize(stream.read_bits(scope.count_bits(variable_type::##name##))); \
               for (auto& var : this->##name##s) \
                  var->read(stream, mp);
            megalo_variable_declaration_set_read_type(scalar);
            megalo_variable_declaration_set_read_type(timer);
            megalo_variable_declaration_set_read_type(team);
            megalo_variable_declaration_set_read_type(player);
            megalo_variable_declaration_set_read_type(object);
            #undef megalo_variable_declaration_set_read_type
         }
         void write(cobb::bitwriter& stream) const noexcept {
            auto& scope = getScopeObjectForConstant(this->type);
            //
            #define megalo_variable_declaration_set_write_type(name) \
               stream.write(this->##name##s.size(), scope.count_bits(variable_type::##name##)); \
               for (auto& var : this->##name##s) \
                  var->write(stream);
            megalo_variable_declaration_set_write_type(scalar);
            megalo_variable_declaration_set_write_type(timer);
            megalo_variable_declaration_set_write_type(team);
            megalo_variable_declaration_set_write_type(player);
            megalo_variable_declaration_set_write_type(object);
            #undef megalo_variable_declaration_set_write_type
         }
         void decompile(Decompiler& out, uint32_t flags = 0) noexcept;
         //
         void adopt(VariableDeclarationSet& other) noexcept;
         bool imply(variable_type vt, uint8_t index) noexcept;
         void make_explicit(variable_type vt, uint8_t index) noexcept;
         //
         VariableDeclaration* get_or_create_declaration(variable_type vt, uint8_t index) noexcept;
   };
}
