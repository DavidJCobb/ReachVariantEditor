#pragma once
#include <cassert>
#include <string>
#include <vector>
#include "../../../formats/detailed_enum.h"

namespace Megalo {
   enum class variable_scope {
      global,
      player,
      object,
      team,
      temporary, // added in MCC (backported from H4)
      //
      not_a_scope = -1,
   };
   enum class variable_type { // must match the game
      scalar = 0,
      player = 1,
      object = 2,
      team   = 3,
      timer  = 4,
      //
      not_a_variable = -1, // needed for "All Players" values in team-or-player vars
   };

   class OpcodeArgTypeinfo;

   class VariableScopeWhichValueList;
   class VariableScopeWhichValue {
      friend VariableScopeWhichValueList;
      public:
         struct flag {
            flag() = delete;
            enum type : uint8_t {
               is_read_only = 0x01,
               is_none      = 0x02,
               is_transient = 0x04,
            };
         };
         using flags_t = std::underlying_type_t<flag::type>;
         //
      protected:
         const VariableScopeWhichValueList* owner = nullptr;
         //
      public:
         std::string name;
         flags_t     flags = 0;
         //
         VariableScopeWhichValue(const char* c, flags_t f = 0) : name(c), flags(f) {}
         //
         inline bool is_none() const noexcept { return this->flags & flag::is_none; }
         inline bool is_read_only() const noexcept { return this->flags & (flag::is_read_only | flag::is_none); }
         //
         int8_t as_integer() const noexcept;
   };
   class VariableScopeWhichValueList {
      private:
         bool _initialized = false;
         //
      public:
         std::vector<VariableScopeWhichValue*> values;
         //
         VariableScopeWhichValueList(std::initializer_list<VariableScopeWhichValue*> e) : values(e) {
            //
            // NOTE: The object must be defined AFTER its contents, or the contents' (owner) 
            // pointer will get wiped due to static variable initialization order! Note my 
            // choice of wording: the DEFINITION, not the DECLARATION, must come after.
            //
            for (auto& v : this->values)
               v->owner = this;
         }
         //
         inline size_t size() const noexcept { return this->values.size(); }
         inline VariableScopeWhichValue& operator[](int i) noexcept { return *this->values[i]; }
         inline const VariableScopeWhichValue& operator[](int i) const noexcept { return *this->values[i]; }
         inline int count_bits() const noexcept {
            auto s = this->size();
            return s ? cobb::bitcount(s) : 0;
         }
         inline int index_bits() const noexcept {
            auto s = this->size();
            return s ? cobb::bitcount(s - 1) : 0;
         }
         //
         int8_t index_of(const VariableScopeWhichValue& v) const noexcept;
   };

   namespace variable_which_values {
      namespace object {
         extern const VariableScopeWhichValueList list;
         //
         extern VariableScopeWhichValue no_object;
         extern VariableScopeWhichValue global_0;
         extern VariableScopeWhichValue global_1;
         extern VariableScopeWhichValue global_2;
         extern VariableScopeWhichValue global_3;
         extern VariableScopeWhichValue global_4;
         extern VariableScopeWhichValue global_5;
         extern VariableScopeWhichValue global_6;
         extern VariableScopeWhichValue global_7;
         extern VariableScopeWhichValue global_8;
         extern VariableScopeWhichValue global_9;
         extern VariableScopeWhichValue global_10;
         extern VariableScopeWhichValue global_11;
         extern VariableScopeWhichValue global_12;
         extern VariableScopeWhichValue global_13;
         extern VariableScopeWhichValue global_14;
         extern VariableScopeWhichValue global_15;
         extern VariableScopeWhichValue current;
         extern VariableScopeWhichValue hud_target;
         extern VariableScopeWhichValue killed;
         extern VariableScopeWhichValue killer;
         extern VariableScopeWhichValue unknown_21;
         extern VariableScopeWhichValue temporary_0;
         extern VariableScopeWhichValue temporary_1;
         extern VariableScopeWhichValue temporary_2;
         extern VariableScopeWhichValue temporary_3;
         extern VariableScopeWhichValue temporary_4;
         extern VariableScopeWhichValue temporary_5;
         extern VariableScopeWhichValue temporary_6;
         extern VariableScopeWhichValue temporary_7;
      }
      namespace player {
         extern const VariableScopeWhichValueList list;
         //
         extern VariableScopeWhichValue no_player;
         extern VariableScopeWhichValue player_0;
         extern VariableScopeWhichValue player_1;
         extern VariableScopeWhichValue player_2;
         extern VariableScopeWhichValue player_3;
         extern VariableScopeWhichValue player_4;
         extern VariableScopeWhichValue player_5;
         extern VariableScopeWhichValue player_6;
         extern VariableScopeWhichValue player_7;
         extern VariableScopeWhichValue player_8;
         extern VariableScopeWhichValue player_9;
         extern VariableScopeWhichValue player_10;
         extern VariableScopeWhichValue player_11;
         extern VariableScopeWhichValue player_12;
         extern VariableScopeWhichValue player_13;
         extern VariableScopeWhichValue player_14;
         extern VariableScopeWhichValue player_15;
         extern VariableScopeWhichValue global_0;
         extern VariableScopeWhichValue global_1;
         extern VariableScopeWhichValue global_2;
         extern VariableScopeWhichValue global_3;
         extern VariableScopeWhichValue global_4;
         extern VariableScopeWhichValue global_5;
         extern VariableScopeWhichValue global_6;
         extern VariableScopeWhichValue global_7;
         extern VariableScopeWhichValue current;
         extern VariableScopeWhichValue hud;
         extern VariableScopeWhichValue hud_target;
         extern VariableScopeWhichValue killer;
         extern VariableScopeWhichValue temporary_0; // added in MCC (backported from H4)
         extern VariableScopeWhichValue temporary_1; // added in MCC (backported from H4)
         extern VariableScopeWhichValue temporary_2; // added in MCC (backported from H4)
      }
      namespace team {
         extern const VariableScopeWhichValueList list;
         //
         extern VariableScopeWhichValue no_team;
         extern VariableScopeWhichValue team_0;
         extern VariableScopeWhichValue team_1;
         extern VariableScopeWhichValue team_2;
         extern VariableScopeWhichValue team_3;
         extern VariableScopeWhichValue team_4;
         extern VariableScopeWhichValue team_5;
         extern VariableScopeWhichValue team_6;
         extern VariableScopeWhichValue team_7;
         extern VariableScopeWhichValue neutral_team;
         extern VariableScopeWhichValue global_0;
         extern VariableScopeWhichValue global_1;
         extern VariableScopeWhichValue global_2;
         extern VariableScopeWhichValue global_3;
         extern VariableScopeWhichValue global_4;
         extern VariableScopeWhichValue global_5;
         extern VariableScopeWhichValue global_6;
         extern VariableScopeWhichValue global_7;
         extern VariableScopeWhichValue current;
         extern VariableScopeWhichValue hud_player_owner_team;
         extern VariableScopeWhichValue hud_target_player_owner_team;
         extern VariableScopeWhichValue temporary_0;
         extern VariableScopeWhichValue temporary_1;
         extern VariableScopeWhichValue temporary_2;
         extern VariableScopeWhichValue temporary_3;
         extern VariableScopeWhichValue temporary_4;
         extern VariableScopeWhichValue temporary_5;
      }
   }
   extern const VariableScopeWhichValueList megalo_scope_does_not_have_specifier; // there is only one Global scope; you don't need to specify "which Global scope" a variable is in

   class VariableScope {
      public:
         constexpr VariableScope(const VariableScopeWhichValueList& list, uint8_t n, uint8_t i, uint8_t e, uint8_t p, uint8_t o) :
            list(list),
            max_scalars(n),
            max_timers(i),
            max_teams(e),
            max_players(p),
            max_objects(o)
         {}
         //
         const VariableScopeWhichValueList& list;
         uint8_t max_scalars;
         uint8_t max_timers;
         uint8_t max_teams;
         uint8_t max_players;
         uint8_t max_objects;
         //
         const int max_variables_of_type(const variable_type vt) const noexcept {
            switch (vt) {
               case variable_type::scalar: return this->max_scalars;
               case variable_type::timer:  return this->max_timers;
               case variable_type::team:   return this->max_teams;
               case variable_type::player: return this->max_players;
               case variable_type::object: return this->max_objects;
            }
            return 0;
         }
         const int which_bits() const noexcept { return this->list.index_bits(); }
         const int count_bits(const variable_type vt) const noexcept {
            return cobb::bitcount(this->max_variables_of_type(vt));
         }
         const int index_bits(const variable_type vt) const noexcept {
            return cobb::bitcount(this->max_variables_of_type(vt) - 1);
         }
         //
         bool is_valid_which(int which) const noexcept {
            auto size = this->list.size();
            if (size == 0)
               return which == 0;
            return which >= 0 && which < size;
         }
   };

   extern const VariableScope MegaloVariableScopeGlobal;
   extern const VariableScope MegaloVariableScopePlayer;
   extern const VariableScope MegaloVariableScopeObject;
   extern const VariableScope MegaloVariableScopeTeam;
   extern const VariableScope MegaloVariableScopeTemporary;

   extern const VariableScope& getScopeObjectForConstant(variable_scope) noexcept;
   extern const VariableScope* getScopeObjectForConstant(variable_type) noexcept;
   extern variable_scope getScopeConstantForObject(const VariableScope&) noexcept;
   //
   extern variable_scope getVariableScopeForTypeinfo(const OpcodeArgTypeinfo*) noexcept;
   extern variable_type  getVariableTypeForTypeinfo(const OpcodeArgTypeinfo*) noexcept;
   extern const VariableScope* getScopeObjectForTypeinfo(const OpcodeArgTypeinfo* ti) noexcept;
}