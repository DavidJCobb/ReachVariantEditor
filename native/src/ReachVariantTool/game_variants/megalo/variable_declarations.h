#pragma once
#include "variables_and_scopes.h"
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
         bool flag = false;
         //
         void read(cobb::bitstream& stream) noexcept {
            if (!this->initial.read(stream))
               return;
            this->networking = (network_type)stream.read_bits(2);
            stream.read(this->flag);
         }
   };
   class PlayerVariableDeclaration {
      public:
         using network_type = variable_network_priority;
         //
         network_type networking = network_type::default;
         bool flag = false;
         //
         void read(cobb::bitstream& stream) noexcept {
            this->networking = (network_type)stream.read_bits(2);
            stream.read(this->flag);
         }
   };
   class ObjectVariableDeclaration {
      public:
         using network_type = variable_network_priority;
         //
         network_type networking = network_type::default;
         bool flag = false;
         //
         void read(cobb::bitstream& stream) noexcept {
            this->networking = (network_type)stream.read_bits(2);
            stream.read(this->flag);
         }
   };
   class TeamVariableDeclaration {
      public:
         using network_type = variable_network_priority;
         //
         network_type networking = network_type::default;
         bool    flag    = false;
         uint8_t initial = 0;
         //
         void read(cobb::bitstream& stream) noexcept {
            this->initial    = stream.read_bits<uint8_t>(4);
            this->networking = (network_type)stream.read_bits(2);
            stream.read(this->flag);
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
            this->scalars.resize(stream.read_bits(scope.count_bits(variable_type::scalar)));
            for (auto& var : this->scalars)
               var.read(stream);
            //
            this->players.resize(stream.read_bits(scope.count_bits(variable_type::player)));
            for (auto& var : this->players)
               var.read(stream);
            //
            this->objects.resize(stream.read_bits(scope.count_bits(variable_type::object)));
            for (auto& var : this->objects)
               var.read(stream);
            //
            this->teams.resize(stream.read_bits(scope.count_bits(variable_type::team)));
            for (auto& var : this->teams)
               var.read(stream);
            //
            this->timers.resize(stream.read_bits(scope.count_bits(variable_type::timer)));
            for (auto& var : this->timers)
               var.read(stream);
         }
   };
}
