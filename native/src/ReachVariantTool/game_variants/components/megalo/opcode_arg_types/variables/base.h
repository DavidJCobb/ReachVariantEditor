#pragma once
#include <functional>
#include <string>
#include "../../variables_and_scopes.h"
#include "../../opcode_arg.h"
#include "../../../../helpers/bitwise.h"
#include "../../../../formats/indexed_lists.h"

/*
// MEGALO VARIABLES
//
// All of the variable types are organized in a common manner: there is a scope-indicator value; 
// a "which" value; and an "index" value.
//
// The scope-indicator is an enum (with different values for each variable type) which indicates 
// where we are pulling a variable from. It can indicate a variable scope (global, object, player, 
// or team), but it can also indicate a property (e.g. a player's owner team) or a game state 
// value (e.g. the round timer, or the friendly fire bool).
//
// The "which" value, when present, indicates which variable serves as our scope. The "index" 
// value, when present, indicates which variable we are accessing. These values are only present 
// if the scope-indicator in question actually uses them.
//
// Some examples:
//
//                                     | SCOPE             | WHICH            | INDEX  | NOTES
//    ---------------------------------+-------------------+------------------+--------+----------------------------------------------------------------------------------------
//    global.number[3]                 | global            | <none>           | 3      | 
//    global.timer[3]                  | global            | <none>           | 3      | 
//    global.object[3]                 | global            | global.object[3] | <none> | If a variable type supports nesting, then its globals use (which) rather than (index). 
//    global.player[1].number[2]       | player            | global.player[1] | 2      | In practice, that would be object, player, and team variables.
//    current_player                   | global            | current_player   | <none> | 
//    current_player.number[2]         | player            | current_player   | 2      | 
//    game.betrayal_booting            | betrayal_booting  | <none>           | <none> | 
//    global.player[1].team            | player_owner_team | global.player[1] | <none> | Note that here, "team"  is a property, not a nested variable.
//    global.player[1].object[0].biped | player_biped      | global.player[1] | 0      | Note that here, "biped" is a property, not a nested variable.
//    55                               | constant          | <none>           | 55     | 
//
// Each variable type has its own enum of scope-indicators, since the scope-indicator does 
// double duty and handles game state values and properties along with nested and non-nested 
// variables. Notably, while all variable types have variable-scopes in their respective scope-
// indicator enums, the variable-scopes aren't always listed in the same order.
//
// The "which" value can only refer to the variable to which access is scoped, i.e. a globally-
// accessible object, player, or team. As such, it is always an entry in the (megalo_objects), 
// (megalo_players), or (megalo_teams) enums defined in <variables_and_scopes.h>.
//
// I've chosen to handle all of this generically. For each variable type I will be defining an 
// instance of VariableScopeIndicatorValueList, which will contain VariableScopeIndicatorValue 
// instances. Each VariableScopeIndicatorValue instance will specify whether and how to handle 
// a "which" value and an "index" value, as well as functors (if needed) for postprocessing.
//
// This entire system differs from what we originally shipped with in the following ways:
//
//  - The old variable system wasn't consistent about when to use (which) versus (index) for 
//    some variables. We now always use (which) to refer to the (megalo_WHATEVERs) enums.
//
*/

namespace Megalo {
   class VariableScopeIndicatorValue {
      public:
         struct flags {
            flags() = delete;
            enum type : uint8_t {
               none = 0,
               is_readonly  = 0x01, // for the compiler; indicates that values in this scope-indicator cannot be written to at run-time
               is_var_scope = 0x02,
            };
         };
         using flags_t = std::underlying_type_t<flags::type>;
         //
         using indexed_access_functor_t = std::function<indexed_list_item* (GameVariantDataMultiplayer& mp, uint32_t index)>; // for indexed data, access the indexed list and return a bare pointer; caller will jam that into a refcounted pointer
         //
         enum class index_type : uint8_t {
            none, // there is no "index" value
            number,
            object,
            player,
            team,
            timer,
            indexed_data, // e.g. script options, script stats; specify bitcount in (index_bitcount)
            generic, // not a variable or an indexed object
         };
         using index_t = index_type;
         //
         flags_t        flags          = flags::none;
         index_type     index_type     = index_type::none;
         uint8_t        index_bitcount = 0; // use non-zero values for index_type::indexed_data and index_type::generic
         const VariableScope* base     = nullptr; // used to deduce whether a value's (which) is from megalo_objects, megalo_players, or megalo_teams
         const char*    format         = "%w.timer[%i]"; // format string for decompiled code; interpreted manually; %w = the "which" as a string; %i = index as a number
         const char*    format_english = "%w's timer[%i]";
         //
         indexed_access_functor_t indexed_list_accessor = nullptr;
         //
         [[nodiscard]] inline bool has_index() const noexcept { return this->index_type != index_type::none; }
         [[nodiscard]] inline bool has_which() const noexcept { return this->base != nullptr; }
         [[nodiscard]] inline bool is_indexed_data() const noexcept { return this->index_type == index_type::indexed_data; }
         [[nodiscard]] inline bool is_readonly() const noexcept { return this->flags & flags::is_readonly; }
         [[nodiscard]] inline bool is_variable_scope() const noexcept { return this->flags & flags::is_var_scope; }
         //
         [[nodiscard]] int which_bits() const noexcept;
         [[nodiscard]] int index_bits() const noexcept;
         //
         VariableScopeIndicatorValue() {}
         VariableScopeIndicatorValue(const char* fd, const char* fe, enum index_type it, uint8_t bc, flags_t flg = 0) : format(fd), format_english(fe), index_type(it), index_bitcount(bc), flags(flg) {}
         VariableScopeIndicatorValue(const char* fd, const char* fe, const VariableScope* which, enum index_type it, flags_t flg = 0) : format(fd), format_english(fe), base(which), index_type(it), flags(flg) {}
         static VariableScopeIndicatorValue make_game_value(const char* fd, const char* fe, uint8_t flg = 0) {
            VariableScopeIndicatorValue result;
            result.flags          = flg;
            result.format         = fd;
            result.format_english = fe;
            return result;
         }
         static VariableScopeIndicatorValue make_indexed_data_indicator(const char* fd, const char* fe, uint8_t bc, indexed_access_functor_t f, flags_t flg = 0) {
            VariableScopeIndicatorValue result;
            result.flags             = flg;
            result.format            = fd;
            result.format_english    = fe;
            result.index_type        = index_type::indexed_data;
            result.index_bitcount    = bc;
            result.indexed_list_accessor = f;
            return result;
         }
         static VariableScopeIndicatorValue make_indexed_data_indicator(const char* fd, const char* fe, const VariableScope* which, uint8_t bc, indexed_access_functor_t f, flags_t flg = 0) {
            VariableScopeIndicatorValue result = make_indexed_data_indicator(fd, fe, bc, f, flg);
            result.base = which;
            return result;
         }
         //
         static VariableScopeIndicatorValue make_variable_scope(const char* fd, const char* fe, enum index_type it, uint8_t bc, flags_t flg = 0) {
            return VariableScopeIndicatorValue(fd, fe, it, bc, flg | flags::is_var_scope);
         }
         static VariableScopeIndicatorValue make_variable_scope(const char* fd, const char* fe, const VariableScope* which, enum index_type it, flags_t flg = 0) {
            return VariableScopeIndicatorValue(fd, fe, which, it, flg | flags::is_var_scope);
         }
   };
   class VariableScopeIndicatorValueList {
      public:
         std::vector<VariableScopeIndicatorValue*> scopes;
         const OpcodeArgTypeinfo& typeinfo;
         variable_type var_type = Megalo::variable_type::not_a_variable;
         //
         inline int scope_bits() const noexcept {
            return cobb::bitcount(this->scopes.size() - 1);
         }
         inline int index_of(const VariableScopeIndicatorValue* iv) const noexcept {
            size_t size = this->scopes.size();
            for (size_t i = 0; i < size; i++)
               if (this->scopes[i] == iv)
                  return i;
            return -1;
         }
         //
         VariableScopeIndicatorValueList(const OpcodeArgTypeinfo& type, Megalo::variable_type vt, std::initializer_list<VariableScopeIndicatorValue*> sl);
         const VariableScopeIndicatorValue& operator[](int i) const noexcept { return *this->scopes[i]; }
         //
         VariableScopeIndicatorValue::index_t get_index_type_for_variable_type() const noexcept;
         const VariableScopeIndicatorValue* get_variable_scope(variable_scope) const noexcept;
   };

   class Variable : public OpcodeArgValue {
      //
      // Base class for variable types. Subclasses basically only need to specify what scope-indicator-value-list 
      // to use, as a constructor argument.
      //
      protected:
         uint32_t _global_index_to_which(uint32_t index, bool is_static) const noexcept;
         static uint32_t _global_index_to_which(const OpcodeArgTypeinfo&, uint32_t index, bool is_static) noexcept;
         //
      public:
         const VariableScopeIndicatorValueList& type;
         //
         const VariableScopeIndicatorValue* scope = nullptr;
         uint8_t which = 0;
         int16_t index = 0;
         cobb::refcount_ptr<indexed_list_item> object;
         //
         Variable(const VariableScopeIndicatorValueList& t) : type(t) {}
         //
         virtual bool read(cobb::ibitreader&, GameVariantDataMultiplayer& mp) noexcept override;
         virtual void write(cobb::bitwriter& stream) const noexcept override;
         virtual void to_string(std::string& out) const noexcept override;
         virtual void configure_with_base(const OpcodeArgBase&) noexcept override {}; // used for bool options so they can stringify intelligently
         virtual void decompile(Decompiler& out, Decompiler::flags_t flags = Decompiler::flags::none) noexcept override;
         virtual arg_compile_result compile(Compiler&, Script::string_scanner&, uint8_t part) noexcept override;
         virtual arg_compile_result compile(Compiler&, Script::VariableReference&, uint8_t part) noexcept override;
         //
         virtual variable_type get_variable_type() const noexcept {
            return this->type.var_type;
         }
         //
         // The (create_zero_or_none) function exists to facilitate implementation of a specific feature 
         // related to opcodes. There are several opcodes that will return a result only if there is a 
         // result to return. The function to get a player's Armor Ability, for example, will only write 
         // to the specified object variable if the player has an Armor Ability; if the player does not, 
         // then the variable is not modified (as opposed to clearing it). The OpcodeFuncToScriptMapping 
         // class allows opcodes to have two names, and offers a flag which indicates alternate behavior 
         // for the second name. This allows us to do this:
         //
         //    some_object = current_player.get_armor_ability()
         //
         // as a shorthand for this:
         //
         //    some_object = no_object
         //    some_object = current_player.try_get_armor_ability()
         //
         // We just compile an assignment to none/zero. Of course, we need to be able to generate and 
         // compile the righthand side of that assignment statement. The naive approach would be to 
         // create a variable of the same type as the target variable, and set the new variable's value 
         // to zero/none... but if an opcode were to have a timer variable as its return type, that 
         // approach would fail, because timer variables can't hold a zero or none value. If you want 
         // to set a timer's value directly, you'd assign a number operand to it.
         //
         // As such: this function, when called on the lefthand side, will create a compiled variable 
         // suitable for use as the righthand side.
         //
         virtual Variable* create_zero_or_none() const noexcept = 0;
   };
}