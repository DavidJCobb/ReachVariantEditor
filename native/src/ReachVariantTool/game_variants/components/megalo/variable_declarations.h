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
   class ScalarVariableDeclaration {
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
         void postprocess(GameVariantDataMultiplayer* newlyLoaded) noexcept {
            this->initial->postprocess(newlyLoaded);
         }
   };
   class PlayerVariableDeclaration {
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
         void postprocess(GameVariantDataMultiplayer* newlyLoaded) noexcept {}
   };
   class ObjectVariableDeclaration {
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
         void postprocess(GameVariantDataMultiplayer* newlyLoaded) noexcept {}
   };
   class TeamVariableDeclaration {
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
         void postprocess(GameVariantDataMultiplayer* newlyLoaded) noexcept {}
   };
   class TimerVariableDeclaration {
      public:
         OpcodeArgValueScalar* initial = new OpcodeArgValueScalar;
         //
         void read(cobb::ibitreader& stream, GameVariantDataMultiplayer& mp) noexcept {
            this->initial->read(stream, mp);
         }
         void write(cobb::bitwriter& stream) const noexcept {
            this->initial->write(stream);
         }
         void postprocess(GameVariantDataMultiplayer* newlyLoaded) noexcept {
            this->initial->postprocess(newlyLoaded);
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
         void postprocess(GameVariantDataMultiplayer* newlyLoaded) noexcept {
            for (auto& var : this->scalars)
               var.postprocess(newlyLoaded);
            for (auto& var : this->players)
               var.postprocess(newlyLoaded);
            for (auto& var : this->objects)
               var.postprocess(newlyLoaded);
            for (auto& var : this->teams)
               var.postprocess(newlyLoaded);
            for (auto& var : this->timers)
               var.postprocess(newlyLoaded);
         }
         void decompile(Decompiler& out, uint32_t flags = 0) noexcept {
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
               if (decl.initial->is_const_zero()) // treat zero as a default that can be omitted
                  continue;
               out.write_line("declare ");
               out.write(scope);
               cobb::sprintf(temp, ".number[%u] = ", i);
               out.write(temp);
               decl.initial->decompile(out, flags);
            }
            //
            // Players and objects cannot have initial values.
            //
            for (size_t i = 0; i < this->teams.size(); ++i) {
               auto& decl = this->teams[i];
               if (decl.initial == const_team::none) // treat no_team as a default that can be omitted
                  continue;
               out.write_line("declare ");
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
   };
}
