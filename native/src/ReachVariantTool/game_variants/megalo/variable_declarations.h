#pragma once
#include "limits.h"
#include "variables_and_scopes.h"
#include "opcode_arg_types/all_enums.h"
#include "opcode_arg_types/scalar.h"
#include "../../helpers/bitstream.h"

namespace Megalo {
   enum class variable_network_priority {
      none, // offline
      low,
      high,
      default,
   };
   //
   class ScalarVariableDeclaration {
      public:
         using network_type = variable_network_priority;
         //
         network_type networking = network_type::default;
         OpcodeArgValueScalar initial;
         //
         void read(cobb::bitstream& stream) noexcept {
            if (!this->initial.read(stream))
               return;
            this->networking = (network_type)stream.read_bits(2);
         }
   };
   class PlayerVariableDeclaration {
      public:
         using network_type = variable_network_priority;
         //
         network_type networking = network_type::default;
         //
         void read(cobb::bitstream& stream) noexcept {
            this->networking = (network_type)stream.read_bits(2);
         }
   };
   class ObjectVariableDeclaration {
      public:
         using network_type = variable_network_priority;
         //
         network_type networking = network_type::default;
         //
         void read(cobb::bitstream& stream) noexcept {
            this->networking = (network_type)stream.read_bits(2);
         }
   };
   class TeamVariableDeclaration {
      public:
         using network_type = variable_network_priority;
         //
         network_type networking = network_type::default;
         const_team   initial    = const_team::none;
         //
         void read(cobb::bitstream& stream) noexcept {
            this->initial    = (const_team)(stream.read_bits(cobb::bitcount(8)) - 1);
            this->networking = (network_type)stream.read_bits(2);
         }
   };
   class TimerVariableDeclaration {
      public:
         OpcodeArgValueScalar initial;
         //
         void read(cobb::bitstream& stream) noexcept {
            this->initial.read(stream);
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
         void read(cobb::bitstream& stream) noexcept {
            auto& scope = getScopeObjectForConstant(this->type);
            //
            #define megalo_variable_declaration_set_read_type(name) \
               this->##name##s.resize(stream.read_bits(scope.count_bits(variable_type::##name##))); \
               for (auto& var : this->##name##s) \
                  var.read(stream);
            megalo_variable_declaration_set_read_type(scalar);
            megalo_variable_declaration_set_read_type(timer);
            megalo_variable_declaration_set_read_type(team);
            megalo_variable_declaration_set_read_type(player);
            megalo_variable_declaration_set_read_type(object);
            #undef megalo_variable_declaration_set_read_type
         }
   };
}
