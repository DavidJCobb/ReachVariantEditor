#pragma once
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
//    indicates which variable to use).
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

enum MegaloValueEnum {
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
enum MegaloValueFlagsMask {
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
   protected:
      ComplexValueSubtype(const char* n) : name(n) {};
      //
   public:
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
      template<typename T> static constexpr const ComplexValueSubtype& constant(const char* n) {
         ComplexValueSubtype instance(n);
         instance.constant_flags     = flags::has_constant | flags::is_read_only | (std::is_signed_v<T> ? flags::constant_is_signed : 0);
         instance.constant_bitlength = cobb::bits_in<T>;
         return instance;
      }
      template<MegaloScopeType st, MegaloVariableType vt> static constexpr const ComplexValueSubtype& variable(const char* n) {
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
         using _MST = MegaloScopeType;    // just a shorthand
         using _MVT = MegaloVariableType; // just a shorthand
         //
         namespace scalar {
            //
            // TODO: The indices are important; can we use a std::array<> instead of a namespace?
            //
            // TODO: Find a way to represent the underlying types, i.e. bools versus numbers, when 
            // the value isn't a script-specified constant (i.e. a read-only game state value).
            //
            extern ComplexValueSubtype const_int         = ComplexValueSubtype::constant<int16_t>("Int16");
            extern ComplexValueSubtype player_number_var = ComplexValueSubtype::variable<_MST::player,  _MVT::number>("Player.Number");
            extern ComplexValueSubtype team_number_var   = ComplexValueSubtype::variable<_MST::team,    _MVT::number>("Team.Number");
            extern ComplexValueSubtype global_number_var = ComplexValueSubtype::variable<_MST::globals, _MVT::number>("Global.Number");
            extern ComplexValueSubtype script_option     = ComplexValueSubtype("User-Defined Option", MegaloValueIndexType::option, MegaloValueIndexQuirk::reference);
            extern ComplexValueSubtype spawn_sequence    = ComplexValueSubtype("Spawn Sequence", MegaloValueEnum::object);
            extern ComplexValueSubtype team_score        = ComplexValueSubtype("Team Score",     MegaloValueEnum::team);
            extern ComplexValueSubtype player_score      = ComplexValueSubtype("Player Score",   MegaloValueEnum::player);
            extern ComplexValueSubtype unknown_09        = ComplexValueSubtype("Unknown 09",     MegaloValueEnum::player);
            extern ComplexValueSubtype player_rating     = ComplexValueSubtype("Player Rating",  MegaloValueEnum::player); // runtime: float converted to int
            extern ComplexValueSubtype player_stat       = ComplexValueSubtype("Player Stat",    MegaloValueEnum::player, MegaloValueIndexType::stat, MegaloValueIndexQuirk::reference);
            extern ComplexValueSubtype team_stat         = ComplexValueSubtype("Team Stat",      MegaloValueEnum::team,   MegaloValueIndexType::stat, MegaloValueIndexQuirk::reference);
            extern ComplexValueSubtype current_round     = ComplexValueSubtype("Current Round", true); // UInt16 extended to int
            extern ComplexValueSubtype symmetry_get      = ComplexValueSubtype("Symmetry (Read-Only)", true); // bool
            extern ComplexValueSubtype symmetry          = ComplexValueSubtype("Symmetry", false); // bool
            extern ComplexValueSubtype score_to_win      = ComplexValueSubtype("Score To Win", true);
            extern ComplexValueSubtype unkF7A6           = ComplexValueSubtype("unkF7A6", false); // bool
            extern ComplexValueSubtype teams_enabled     = ComplexValueSubtype("Teams Enabled", false); // bool
            extern ComplexValueSubtype round_time_limit  = ComplexValueSubtype("Round Time Limit", true);
            extern ComplexValueSubtype round_limit       = ComplexValueSubtype("Round Limit", true);
            extern ComplexValueSubtype misc_unk0_bit3    = ComplexValueSubtype("MiscOptions.unk0.bit3", false); // bool
            extern ComplexValueSubtype rounds_to_win     = ComplexValueSubtype("Round Victories To Win", true);
            extern ComplexValueSubtype sudden_death_time = ComplexValueSubtype("Sudden Death Time Limit", true);
            extern ComplexValueSubtype grace_period      = ComplexValueSubtype("Grace Period", true);
            extern ComplexValueSubtype lives_per_round   = ComplexValueSubtype("Lives Per Round", true);
            extern ComplexValueSubtype team_lives_per_round = ComplexValueSubtype("Team Lives Per Round", true);
            extern ComplexValueSubtype respawn_time      = ComplexValueSubtype("Respawn Time", true);
            extern ComplexValueSubtype suicide_penalty   = ComplexValueSubtype("Suicide Penalty", true);
            extern ComplexValueSubtype betrayal_penalty  = ComplexValueSubtype("Betrayal Penalty", true);
            extern ComplexValueSubtype respawn_growth    = ComplexValueSubtype("Respawn Growth", true);
            extern ComplexValueSubtype loadout_cam_time  = ComplexValueSubtype("Loadout Camera Time", true);
            extern ComplexValueSubtype respawn_traits_duration = ComplexValueSubtype("Respawn Traits Duration", true);
            extern ComplexValueSubtype friendly_fire     = ComplexValueSubtype("Friendly Fire", true); // bool
            extern ComplexValueSubtype betrayal_booting  = ComplexValueSubtype("Betrayal Booting", true); // bool
            extern ComplexValueSubtype social_flag_2     = ComplexValueSubtype("Social Flags Bit 2", true); // bool
            extern ComplexValueSubtype social_flag_3     = ComplexValueSubtype("Social Flags Bit 3", true); // bool
            extern ComplexValueSubtype social_flag_4     = ComplexValueSubtype("Social Flags Bit 4", true); // bool
            extern ComplexValueSubtype grenades_on_map   = ComplexValueSubtype("Grenades On Map", true); // bool
            extern ComplexValueSubtype indestructible_vehicles = ComplexValueSubtype("Indestructible Vehicles", true); // bool
            extern ComplexValueSubtype powerup_duration_red    = ComplexValueSubtype("Powerup Duration, Red", true);
            extern ComplexValueSubtype powerup_duration_blue   = ComplexValueSubtype("Powerup Duration, Blue", true);
            extern ComplexValueSubtype powerup_duration_yellow = ComplexValueSubtype("Powerup Duration, Yellow", true);
            extern ComplexValueSubtype death_event_damage_type = ComplexValueSubtype("Death Event Damage Type", true); // byte
         }
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
   public:
      MegaloValueUnderlyingType underlying_type;
      MegaloValueEnum       enumeration = MegaloValueEnum::not_applicable;      // only if underlying type is "enumeration"
      MegaloValueFlagsMask  flags_mask  = MegaloValueFlagsMask::not_applicable; // only if underlying type is "flags"
      MegaloVariableType    var_type    = MegaloVariableType::not_applicable;   // only if underlying type is "variable"
      MegaloValueIndexType  index_type  = MegaloValueIndexType::not_applicable; // only if underlying type is "index"
      MegaloValueIndexQuirk index_quirk = MegaloValueIndexQuirk::none;          // only if underlying type is "index" -- describes any special encoding (akin to what cobb::bitnumber handles)
      //
      MegaloValueType(MegaloValueUnderlyingType ut) : underlying_type(ut) {};
      MegaloValueType(MegaloValueEnum e) : underlying_type(MegaloValueUnderlyingType::enumeration), enumeration(e) {};
      MegaloValueType(MegaloValueFlagsMask f) : underlying_type(MegaloValueUnderlyingType::flags), flags_mask(f) {};
      MegaloValueType(MegaloVariableType vt) : underlying_type(MegaloValueUnderlyingType::variable), var_type(vt) {};
};

namespace reach {
   namespace value_types {
      MegaloValueType boolean           = MegaloValueType(MegaloValueUnderlyingType::boolean);
      MegaloValueType compare_operator  = MegaloValueType(MegaloValueEnum::compare_operator);
      MegaloValueType killer_type_flags = MegaloValueType(MegaloValueFlagsMask::killer_type);
      MegaloValueType explicit_object   = MegaloValueType(MegaloValueEnum::object);
      MegaloValueType explicit_player   = MegaloValueType(MegaloValueEnum::player);
      MegaloValueType explicit_team     = MegaloValueType(MegaloValueEnum::team);
      MegaloValueType team_disposition  = MegaloValueType(MegaloValueEnum::team_disposition);
      MegaloValueType variable_any      = MegaloValueType(MegaloVariableType::any);
      MegaloValueType variable_object   = MegaloValueType(MegaloVariableType::object);
      MegaloValueType variable_player   = MegaloValueType(MegaloVariableType::player);
      MegaloValueType variable_team     = MegaloValueType(MegaloVariableType::team);
      MegaloValueType variable_timer    = MegaloValueType(MegaloVariableType::timer);
   }
}

class MegaloValue {
   public:
      MegaloValueType* type = nullptr;
      RawMegaloValue   value;
      //
      bool read(cobb::bitstream&) noexcept;
};