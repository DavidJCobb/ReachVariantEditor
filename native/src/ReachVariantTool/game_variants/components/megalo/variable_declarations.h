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
         struct compile_flags {
            enum type : uint8_t {
               none = 0,
               implicit = 0x01,
            };
         };
         using compile_flags_t = std::underlying_type_t<compile_flags::type>;
         //
      public:
         compile_flags_t compiler_flags = compile_flags::none;
         //
         inline bool is_implicit() const noexcept { return this->compiler_flags & compile_flags::implicit; }
         inline void make_explicit() noexcept { this->compiler_flags &= ~compile_flags::implicit; }
   };
   class ScalarVariableDeclaration : public VariableDeclaration {
      public:
         using network_enum = variable_network_priority;
         using network_type = variable_network_priority_t;
         //
         network_type networking = network_enum::default;
         OpcodeArgValueScalar* initial = new OpcodeArgValueScalar;
         //
         void read(cobb::ibitreader& stream, GameVariantDataMultiplayer& mp) noexcept {
            if (!this->initial->read(stream, mp))
               return;
            this->networking.read(stream);
         }
         void write(cobb::bitwriter& stream) const noexcept {
            this->initial->write(stream);
            this->networking.write(stream);
         }
   };
   class PlayerVariableDeclaration : public VariableDeclaration {
      public:
         using network_enum = variable_network_priority;
         using network_type = variable_network_priority_t;
         //
         network_type networking = network_enum::default;
         //
         void read(cobb::ibitreader& stream, GameVariantDataMultiplayer& mp) noexcept {
            this->networking.read(stream);
         }
         void write(cobb::bitwriter& stream) const noexcept {
            this->networking.write(stream);
         }
   };
   class ObjectVariableDeclaration : public VariableDeclaration {
      public:
         using network_enum = variable_network_priority;
         using network_type = variable_network_priority_t;
         //
         network_type networking = network_enum::default;
         //
         void read(cobb::ibitreader& stream, GameVariantDataMultiplayer& mp) noexcept {
            this->networking.read(stream);
         }
         void write(cobb::bitwriter& stream) const noexcept {
            this->networking.write(stream);
         }
   };
   class TeamVariableDeclaration : public VariableDeclaration {
      public:
         using network_enum   = variable_network_priority;
         using network_type   = variable_network_priority_t;
         using team_bitnumber = cobb::bitnumber<cobb::bitcount(8), const_team, true>;
         //
         network_type   networking = network_enum::default;
         team_bitnumber initial    = const_team::none;
         //
         void read(cobb::ibitreader& stream, GameVariantDataMultiplayer& mp) noexcept {
            this->initial.read(stream);
            this->networking.read(stream);
         }
         void write(cobb::bitwriter& stream) const noexcept {
            this->initial.write(stream);
            this->networking.write(stream);
         }
   };
   class TimerVariableDeclaration : public VariableDeclaration {
      public:
         OpcodeArgValueScalar* initial = new OpcodeArgValueScalar;
         //
         void read(cobb::ibitreader& stream, GameVariantDataMultiplayer& mp) noexcept {
            this->initial->read(stream, mp);
         }
         void write(cobb::bitwriter& stream) const noexcept {
            this->initial->write(stream);
         }
   };

   class VariableDeclarationSet {
      public:
         variable_scope type;
         std::vector<ScalarVariableDeclaration> scalars;
         std::vector<PlayerVariableDeclaration> players;
         std::vector<ObjectVariableDeclaration> objects;
         std::vector<TeamVariableDeclaration>   teams;
         std::vector<TimerVariableDeclaration>  timers;
         //
         VariableDeclarationSet(variable_scope t) : type(t) {}
         //
         void read(cobb::ibitreader& stream, GameVariantDataMultiplayer& mp) noexcept {
            auto& scope = getScopeObjectForConstant(this->type);
            //
            #define megalo_variable_declaration_set_read_type(name) \
               this->##name##s.resize(stream.read_bits(scope.count_bits(variable_type::##name##))); \
               for (auto& var : this->##name##s) \
                  var.read(stream, mp);
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
                  var.write(stream);
            megalo_variable_declaration_set_write_type(scalar);
            megalo_variable_declaration_set_write_type(timer);
            megalo_variable_declaration_set_write_type(team);
            megalo_variable_declaration_set_write_type(player);
            megalo_variable_declaration_set_write_type(object);
            #undef megalo_variable_declaration_set_write_type
         }
         void decompile(Decompiler& out, uint32_t flags = 0) noexcept;
         //
         bool imply(variable_type vt, uint8_t index) noexcept;
         void make_explicit(variable_type vt, uint8_t index) noexcept;
   };
}
