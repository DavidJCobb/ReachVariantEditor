#pragma once
#include <array>
#include <cstdint>
#include "../../helpers/bitwise.h"
#include "../../helpers/bitstream.h"
#include "../../helpers/bitnumber.h"

// VarRefTypes:
// Custom: indices need to go up to 0x2B; number of possible members is 0x2C
// Player: indices need to go up to 3; number of possible members is 4
// Object: indices need to go up to 7; number of possible members is 8
// Team:   indices need to go up to 5; number of possible members is 6
// Timer:  indices need to go up to 6; number of possible members is 7

//
// VarRefTypes:
//
//  - Fundamental types for variables, e.g. "Custom" (used for numbers and bools) 
//    and "Player"
//
//  - Can be a single enum (e.g. for predefined variables like Score to Win), a 
//    constant, or an enum paired with a constant (for scoped variables; the constant 
//    indicates which variable to use), or neither (for game state variables).
//
// Enums and flags are not VarRefTypes. Some bools are not VarRefTypes. Constants, 
// variables, and game state values like Score To Win are VarRefTypes.
//
// The nomenclature I'm gonna go with is "complex types," which can have "subtypes," 
// and "simple types." (Complex types would be my analogue to VarRefTypes; subtypes, 
// to VarRefTypes' members.) If an opcode argument is of a complex type, then that 
// means that you can specify any of the subtypes as its value; if it's of a simple 
// type, then you specify THAT TYPE as the value. Think of a complex type, then, as a 
// type that can take multiple forms depending on what you're referencing: a complex 
// number can be a constant, a variable, or a reference to any of a number of game 
// state values.
//
// Let's use a specific example: a "compare" condition, which compares two scalars 
// (i.e. numbers or bools). Either scalar can be a constant number, a number variable, 
// or a game state value. So you can compare the Rounds To Win to a constant, or 
// compare the fifth number variable on a player to the Score To Win. This is a 
// complex type, and each of the things it can be -- variable, constant, game state 
// value -- is a subtype.
//
// Each complex subtype has an ID number and can optionally have an enum and a 
// constant.
//
//  - A constant integer has just a constant.
//
//  - A variable has an enum and a constant. If I want to access Player.Number[5], 
//    the enum indicates which player I'm accessing that on (e.g. Player 1, or a 
//    player variable, or "current player"), and the constant indicates which number 
//    variable on that player I'm accessing.
//
//  - A game state value has neither an enum nor a constant. If we know that you're 
//    referring to the "Score To Win" subtype, then that's all we need to know.
//
// The overwhelming majority of opcode arguments are complex-type arguments, it seems.
//
// TODO:
//
//  - There is a hardcoded VarRefType in KSoft.Tool called "Any." How is that loaded? 
//    It wouldn't be enough to load the subtype ID, enum, and constant, because you'd 
//    have to know what the complex type itself is (e.g. is it a number, or a player, 
//    or what?).
//
//  - Currently we've organized ComplexValueSubtype instances into namespaces. However, 
//    complex types are kind of like enums: each subtype has a numeric index; when you 
//    read a complex value, you do so by first reading that index to know what subtype 
//    the value has, and then you read it in the manner appropriate for the subtype. 
//    As such, we should probably use std::array<ComplexValueSubtype, howeverMany> 
//    instead of namespaces.
//
//     - The goal is to be able to do, like, "Oh, we know it's a scalar, so read the 
//       type and do scalar_types[type]."
//
//  - Create a struct, ComplexValue, that has a reference to a ComplexValueSubtype, 
//    a uint32_t for the enum value, and a uint32_t for the constant value.
//

struct ComplexValue {
};

enum class MegaloScopeType {
   // values in this enum aren't intended to match with what the game engine uses
   not_applicable = -1,
   globals = 0,
   player  = 1,
   object  = 2,
   team    = 3,
};
enum class MegaloVariableType {
   // values in this enum aren't intended to match with what the game engine uses
   not_applicable = -2,
   any            = -1, // only for opcode argument definitions; not for loaded values
   number = 0,
   timer  = 1,
   team   = 2,
   player = 3,
   object = 4,
};
enum class MegaloValueUnderlyingType {
   boolean,
   enumeration,
   flags,
   index,
   integer_signed,
   integer_unsigned,
   variable,
};

enum class MegaloValueIndexType {
   not_applicable = -1,
   undefined, // KSoft.Tool has this. what is it? is it used?
   trigger,
   object_type,
   name,
   sound,
   incident,
   icon,
   // Halo 4: medal
   // Halo 4: ordnance
   loadout_palette,
   option,
   string,
   player_traits,
   stat,
   widget,
   object_filter,
   // Halo 4: game object filter
};
enum class MegaloValueIndexQuirk {
   none,
   offset, // add 1 before encoding to avoid sign issues; subtract 1 after decoding
   presence, // a bit indicates whether a value is even present; if absent, use -1
   reference, // value cannot be "none"
   word, // UInt16
};

enum class MegaloValueEnum {
   not_applicable = -1,
   //
   add_weapon_mode,
   c_hud_destination,
   compare_operator,
   drop_weapon_mode,
   grenade_type,
   math_operator,
   object,
   pickup_priority,
   player,
   team,
   team_disposition,
   waypoint_icon,
   waypoint_priority,
};
enum class MegaloValueFlagsMask {
   not_applicable = -1,
   //
   create_object_flags,
   killer_type,
   player_unused_mode_flags,
};

class MegaloScope {
   //
   // All Megalo variables exist in a scope: they can exist as globals, or they can be attached to 
   // specific players, specific objects, or specific teams. Each scope supports only a limited 
   // number of each type of variable, and these limits vary between scopes.
   //
   public:
      using scope_type = MegaloScopeType;
      //
      const scope_type type = scope_type::globals;
      const uint8_t max_numbers;
      const uint8_t max_timers;
      const uint8_t max_teams;
      const uint8_t max_players;
      const uint8_t max_objects;
      //
      constexpr MegaloScope(scope_type a, uint8_t n, uint8_t ti, uint8_t te, uint8_t p, uint8_t o) :
         type(a),
         max_numbers(n),
         max_timers(ti),
         max_teams(te),
         max_players(p),
         max_objects(o)
      {};
      //
   protected:
      static constexpr int _get_count_bitlength(uint8_t max) noexcept { return cobb::bitcount(max); };
      static constexpr int _get_index_bitlength(uint8_t max) noexcept { return cobb::bitcount(max - 1); };
      //
   public:
      constexpr int get_number_count_bitlength() const noexcept { return _get_count_bitlength(this->max_numbers); };
      constexpr int get_number_index_bitlength() const noexcept { return _get_index_bitlength(this->max_numbers); };
      constexpr int get_timer_count_bitlength() const noexcept { return _get_count_bitlength(this->max_timers); };
      constexpr int get_timer_index_bitlength() const noexcept { return _get_index_bitlength(this->max_timers); };
      constexpr int get_team_count_bitlength() const noexcept { return _get_count_bitlength(this->max_teams); };
      constexpr int get_team_index_bitlength() const noexcept { return _get_index_bitlength(this->max_teams); };
      constexpr int get_player_count_bitlength() const noexcept { return _get_count_bitlength(this->max_players); };
      constexpr int get_player_index_bitlength() const noexcept { return _get_index_bitlength(this->max_players); };
      constexpr int get_object_count_bitlength() const noexcept { return _get_count_bitlength(this->max_objects); };
      constexpr int get_object_index_bitlength() const noexcept { return _get_index_bitlength(this->max_objects); };
      //
      template<MegaloVariableType vt> constexpr int get_count_bitlength() const noexcept;
      template<> constexpr int get_count_bitlength<MegaloVariableType::number>() const noexcept { return this->get_number_count_bitlength(); };
      template<> constexpr int get_count_bitlength<MegaloVariableType::timer>()  const noexcept { return this->get_timer_count_bitlength(); };
      template<> constexpr int get_count_bitlength<MegaloVariableType::team>()   const noexcept { return this->get_team_count_bitlength(); };
      template<> constexpr int get_count_bitlength<MegaloVariableType::player>() const noexcept { return this->get_player_count_bitlength(); };
      template<> constexpr int get_count_bitlength<MegaloVariableType::object>() const noexcept { return this->get_object_count_bitlength(); };
      //
      template<MegaloVariableType vt> constexpr int get_index_bitlength() const noexcept;
      template<> constexpr int get_index_bitlength<MegaloVariableType::number>() const noexcept { return this->get_number_index_bitlength(); };
      template<> constexpr int get_index_bitlength<MegaloVariableType::timer>()  const noexcept { return this->get_timer_index_bitlength(); };
      template<> constexpr int get_index_bitlength<MegaloVariableType::team>()   const noexcept { return this->get_team_index_bitlength(); };
      template<> constexpr int get_index_bitlength<MegaloVariableType::player>() const noexcept { return this->get_player_index_bitlength(); };
      template<> constexpr int get_index_bitlength<MegaloVariableType::object>() const noexcept { return this->get_object_index_bitlength(); };
};
namespace reach {
   namespace megalo_scopes {
      extern constexpr MegaloScope not_applicable = MegaloScope(MegaloScopeType::not_applicable, 0, 0, 0, 0, 0);
      //
      extern constexpr MegaloScope global = MegaloScope(MegaloScopeType::globals, 12, 8, 8, 8, 16);
      extern constexpr MegaloScope player = MegaloScope(MegaloScopeType::player,   8, 4, 4, 4,  4);
      extern constexpr MegaloScope object = MegaloScope(MegaloScopeType::object,   8, 4, 2, 4,  4);
      extern constexpr MegaloScope team   = MegaloScope(MegaloScopeType::team,     8, 4, 4, 4,  6);
      //
      template<MegaloScopeType st> constexpr const MegaloScope& get() noexcept { return not_applicable; }
      template<> constexpr const MegaloScope& get<MegaloScopeType::globals>() noexcept { return global; }
      template<> constexpr const MegaloScope& get<MegaloScopeType::player>()  noexcept { return player; }
      template<> constexpr const MegaloScope& get<MegaloScopeType::object>()  noexcept { return object; }
      template<> constexpr const MegaloScope& get<MegaloScopeType::team>()    noexcept { return team; }
      //
      constexpr const MegaloScope& get(MegaloScopeType st) noexcept {
         switch (st) {
            case MegaloScopeType::globals:
               return global;
            case MegaloScopeType::player:
               return player;
            case MegaloScopeType::object:
               return object;
            case MegaloScopeType::team:
               return team;
            case MegaloScopeType::not_applicable:
            default:
               return not_applicable;
         }
      };
   };
   namespace megalo_value_flags {
      struct create_object_flags {
         create_object_flags() = delete;
         enum {
            never_garbage_collect,
            unk_1,
            unk_2,
         };
      };
      struct killer_type {
         killer_type() = delete;
         enum {
            guardians,
            suicide,
            kill,
            betrayal,
            quit,
         };
      };
      struct player_unused_mode_flags {
         player_unused_mode_flags() = delete;
         enum {
            unk_0,
            unk_1,
            unk_2,
            unk_3,
         };
      };
   }
   namespace megalo_value_enums {
      enum class add_weapon_mode {
         add,
         unk_1,
         swap,
      };
      enum class c_hud_destination {
         unk_0,
         unk_1,
      };
      enum class compare_operator { // 3 bits
         less,
         greater,
         equal,
         less_or_equal,
         greater_or_equal,
         not_equal,
      };
      enum class drop_weapon_mode {
         primary,
         secondary,
      };
      enum class grenade_type {
         frag,
         plasma,
      };
      enum class math_operator {
         add,
         subtract,
         multiply,
         divide,
         assign,
         modulo,
         bitwise_and,
         bitwise_or,
         bitwise_xor,
         bitwise_not,
         bitwise_shift_left,
         bitwise_shift_right,
         bitwise_shift_left_2, // not sure what the difference here is
      };
      enum class object {
         none = 0,
         global_object_0,
         global_object_1,
         global_object_2,
         global_object_3,
         global_object_4,
         global_object_5,
         global_object_6,
         global_object_7,
         global_object_8,
         global_object_9,
         global_object_10,
         global_object_11,
         global_object_12,
         global_object_13,
         global_object_14,
         global_object_15,
         current_object, // iterator
         hud_target, // HUD interface
         killed, // object death event
         killer, // object death event
      };
      enum class pickup_priority {
         unk_0,
         hold_action,
         automatic,
         unk_3,
      };
      enum class player {
         none     = 0,
         player_0,
         player_1,
         player_2,
         player_3,
         player_4,
         player_5,
         player_6,
         player_7,
         player_8,
         player_9,
         player_10,
         player_11,
         player_12,
         player_13,
         player_14,
         player_15,
         global_player_0,
         global_player_1,
         global_player_2,
         global_player_3,
         global_player_4,
         global_player_5,
         global_player_6,
         global_player_7,
         current_player, // iterator
         hud_player, // HUD interface
         hud_target, // HUD interface
         killer // object death event
      };
      enum class team {
         none = -1, // TODO: verify
         team_0,
         team_1,
         team_2,
         team_3,
         team_4,
         team_5,
         team_6,
         team_7,
         neutral,
         global_team_0,
         global_team_1,
         global_team_2,
         global_team_3,
         global_team_4,
         global_team_5,
         global_team_6,
         global_team_7,
         current_team, // iterator
         hud_player, // HUD interface
         hud_target, // HUD interface
         // it's not known precisely where this enum ends
         unk_14,
         unk_15,
      };
      enum class team_disposition {
         unk_0,
         unk_1,
         unk_2,
      };
      enum class waypoint_icon { // 5 bits
         none = -1,
         microphone,
         death_marker,
         lightning_bolt,
         bullseye,
         diamond,
         bomb,
         flag,
         skull,
         crown,
         vip,
         territory_lock,
         territory_a,
         territory_b,
         territory_c,
         territory_d,
         territory_e,
         territory_f,
         territory_g,
         territory_h,
         territory_i,
         supply,
         supply_health,
         supply_air_drop,
         supply_ammo,
         arrow,
         defend,
         unk_26,
         unk_27,
      };
      enum class waypoint_priority {
         none,
         low,
         high,
         default,
      };
   };
   //
   template<MegaloScopeType st, MegaloVariableType vt> struct _bitlength_for_variable_index {
      inline static constexpr const MegaloScope& scope = megalo_scopes::get<st>();
      inline static constexpr const int          value = scope.get_index_bitlength<vt>();
   };
   template<MegaloScopeType st, MegaloVariableType vt> inline constexpr int bitlength_for_variable_index = _bitlength_for_variable_index<st, vt>::value;
}

namespace reach {
   template<MegaloScopeType st> struct _megalo_enum_for_scope_type {
      static constexpr MegaloValueEnum value = MegaloValueEnum::not_applicable;
   };
   template<> struct _megalo_enum_for_scope_type<MegaloScopeType::object> {
      static constexpr MegaloValueEnum value = MegaloValueEnum::object;
   };
   template<> struct _megalo_enum_for_scope_type<MegaloScopeType::player> {
      static constexpr MegaloValueEnum value = MegaloValueEnum::player;
   };
   template<> struct _megalo_enum_for_scope_type<MegaloScopeType::team> {
      static constexpr MegaloValueEnum value = MegaloValueEnum::team;
   };
   template<MegaloScopeType st> inline constexpr MegaloValueEnum megalo_enum_for_scope_type = _megalo_enum_for_scope_type<st>::value;
}

class ComplexValueSubtype {
   public:
      struct flags {
         flags() = delete;
         enum : uint8_t {
            has_constant       = 0x01,
            constant_is_signed = 0x02,
            is_read_only       = 0x04,
            has_index          = 0x08,
         };
      };
      //
      const char*     name;
      MegaloValueEnum enumeration = MegaloValueEnum::not_applicable;
      uint8_t constant_flags     = 0;
      int     constant_bitlength = 0;
      MegaloValueIndexType  index_type  = MegaloValueIndexType::not_applicable;
      MegaloValueIndexQuirk index_quirk = MegaloValueIndexQuirk::none;
      //
   public:
      constexpr ComplexValueSubtype(const char* n) : name(n) {}; // needs to be public so 
      constexpr ComplexValueSubtype(const char* n, bool readonly) : name(n), constant_flags(readonly ? flags::is_read_only : 0) {};
      constexpr ComplexValueSubtype(const char* n, MegaloValueEnum e) : name(n), enumeration(e), constant_flags(flags::is_read_only) {};
      constexpr ComplexValueSubtype(const char* n, MegaloValueEnum e, uint8_t f) : name(n), enumeration(e), constant_flags(f | flags::is_read_only) {};
      constexpr ComplexValueSubtype(const char* n, MegaloValueIndexType it, MegaloValueIndexQuirk iq) :
         name(n),
         constant_flags(flags::has_index | flags::is_read_only),
         index_type(it),
         index_quirk(iq)
      {};
      constexpr ComplexValueSubtype(const char* n, MegaloValueEnum e, MegaloValueIndexType it, MegaloValueIndexQuirk iq) :
         name(n),
         enumeration(e),
         constant_flags(flags::has_index | flags::is_read_only),
         index_type(it),
         index_quirk(iq)
      {};
      //
      template<typename T> static constexpr const ComplexValueSubtype constant(const char* n) {
         ComplexValueSubtype instance(n);
         instance.constant_flags     = flags::has_constant | flags::is_read_only | (std::is_signed_v<T> ? flags::constant_is_signed : 0);
         instance.constant_bitlength = cobb::bits_in<T>;
         return instance;
      }
      template<MegaloScopeType st, MegaloVariableType vt> static constexpr const ComplexValueSubtype variable(const char* n) {
         ComplexValueSubtype instance(n);
         instance.enumeration        = reach::megalo_enum_for_scope_type<st>; // falls back to not_applicable as appropriate
         instance.constant_flags     = flags::has_constant;
         instance.constant_bitlength = reach::bitlength_for_variable_index<st, vt>;
         return instance;
      }
      //
      // TO READ ONE OF THESE FROM A SCRIPT'S DATA:
      //
      // // given std::array<ComplexValueSubtype, num> subtypes;
      // // given type_bitlength = cobb::bitcount<num - 1>;
      // // given some object (myValue) that can hold subtype info
      //
      // int type = stream.read_bits<uint8_t>(type_bitlength);
      // if (type >= subtypes.size())
      //    assert(false && "handle this error");
      // ComplexValueSubtype& subtype = subtypes[type];
      // if (subtype.enumeration != MegaloValueEnum::not_applicable) {
      //    myValue.enumValue = ...; // read a number of bits dependent on the enum
      //    // TODO: this applies to flags masks as well; we need to add support for that
      // }
      // if (subtype.constant_flags & flags::has_constant) {
      //    myValue.constant = stream.read_bits(subtype.constant_bitlength);
      // } else if (subtype.constant_flags & flags::has_index) {
      //    // TODO: read the index accounting for the MegaloValueIndexQuirk
      // }
      //
};
namespace reach {
   namespace megalo {
      namespace complex_values {
         //
         //
         // TODO: The indices are important; can we use a std::array<> instead of a namespace? 
         // We should create an enum alongside it just so we remember everything.
         //
         // TODO: Ditch the static templated methods in favor of constructor overloads. We only 
         // went with the static methods in the first place because constructor overloads were 
         // failing on constexpr (turns out, things have to be constexpr AND const to truly be 
         // constexpr, because this language is odd).
         //
         // TODO: Find a way to represent the underlying types, i.e. bools versus numbers, when 
         // the value isn't a script-specified constant (i.e. a read-only game state value).
         //
         extern std::array<ComplexValueSubtype, 43> scalar;
         namespace player {
         }
         namespace object {
         }
         namespace team {
         }
         namespace timer{
         }
      }
   }
}

// A variable reference e.g. Player.Numeric 5 is:
// [reference kind][index bitcount: enum VariableReferenceTypes.Custom][DataType, if any][DataValue, if any]

struct ReachVector3 { // TODO: bring in cobb::vector3<float>
   float x;
   float y;
   float z;
};
struct RawMegaloVariableReferencingValue {
   MegaloScopeType    scope;
   MegaloVariableType type;
   uint8_t            which = 0;
};
union RawMegaloValue {
   bool         boolean;
   int32_t      integer_signed;
   uint32_t     integer_unsigned = 0;
   ReachVector3 vector3;
   RawMegaloVariableReferencingValue variable;
};

class MegaloValueType {
   //
   // this may be somewhat outdated? it was written before I came to understand complex and simple types
   //
   public:
      MegaloValueUnderlyingType underlying_type;
      MegaloValueEnum       enumeration = MegaloValueEnum::not_applicable;      // only if underlying type is "enumeration"
      MegaloValueFlagsMask  flags_mask  = MegaloValueFlagsMask::not_applicable; // only if underlying type is "flags"
      MegaloVariableType    var_type    = MegaloVariableType::not_applicable;   // only if underlying type is "variable"
      MegaloValueIndexType  index_type  = MegaloValueIndexType::not_applicable; // only if underlying type is "index"
      MegaloValueIndexQuirk index_quirk = MegaloValueIndexQuirk::none;          // only if underlying type is "index" -- describes any special encoding (akin to what cobb::bitnumber handles)
      //
      constexpr MegaloValueType(MegaloValueUnderlyingType ut) : underlying_type(ut) {};
      constexpr MegaloValueType(MegaloValueEnum e) : underlying_type(MegaloValueUnderlyingType::enumeration), enumeration(e) {};
      constexpr MegaloValueType(MegaloValueFlagsMask f) : underlying_type(MegaloValueUnderlyingType::flags), flags_mask(f) {};
      constexpr MegaloValueType(MegaloVariableType vt) : underlying_type(MegaloValueUnderlyingType::variable), var_type(vt) {};
};

namespace reach {
   namespace value_types {
      inline constexpr MegaloValueType boolean           = MegaloValueType(MegaloValueUnderlyingType::boolean);
      inline constexpr MegaloValueType compare_operator  = MegaloValueType(MegaloValueEnum::compare_operator);
      inline constexpr MegaloValueType killer_type_flags = MegaloValueType(MegaloValueFlagsMask::killer_type);
      inline constexpr MegaloValueType explicit_object   = MegaloValueType(MegaloValueEnum::object);
      inline constexpr MegaloValueType explicit_player   = MegaloValueType(MegaloValueEnum::player);
      inline constexpr MegaloValueType explicit_team     = MegaloValueType(MegaloValueEnum::team);
      inline constexpr MegaloValueType team_disposition  = MegaloValueType(MegaloValueEnum::team_disposition);
      inline constexpr MegaloValueType variable_any      = MegaloValueType(MegaloVariableType::any);
      inline constexpr MegaloValueType variable_object   = MegaloValueType(MegaloVariableType::object);
      inline constexpr MegaloValueType variable_player   = MegaloValueType(MegaloVariableType::player);
      inline constexpr MegaloValueType variable_team     = MegaloValueType(MegaloVariableType::team);
      inline constexpr MegaloValueType variable_timer    = MegaloValueType(MegaloVariableType::timer);
   }
}

class MegaloValue {
   public:
      MegaloValueType* type = nullptr;
      RawMegaloValue   value;
      //
      void read(cobb::bitstream&) noexcept;
};