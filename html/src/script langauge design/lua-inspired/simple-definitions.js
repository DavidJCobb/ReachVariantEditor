const var_scope = _make_enum([
   "global",
   "object",
   "player",
   "team",
]);
const var_type = _make_enum([
   "number",
   "object",
   "player",
   "team",
   "timer",
]);
function var_type_to_var_scope(type) {
   switch (type) {
      case var_type.number: return null;
      case var_type.object: return var_scope.object;
      case var_type.player: return var_scope.player;
      case var_type.team:   return var_scope.team;
      case var_type.timer:  return null;
   }
   return null;
}
//
const number_var_scope = _make_enum([
   "constant",
   "player_number",
   "object_number",
   "team_number",
   "global_number",
   "script_option",
   "spawn_sequence",
   "team_score",
   "player_score",
   "unknown_09", // unused in Reach? used in Halo 4?
   "player_rating",
   "player_stat",
   "team_stat",
   "current_round",
   "symmetry_getter",
   "symmetry",
   "score_to_win",
   "unkF7A6",
   "teams_enabled",
   "round_time_limit",
   "round_limit",
   "misc_unk0_bit3",
   "rounds_to_win",
   "sudden_death_time",
   "grace_period",
   "lives_per_round",
   "team_lives_per_round",
   "respawn_time",
   "suicide_penalty",
   "betrayal_penalty",
   "respawn_growth",
   "loadout_cam_time",
   "respawn_traits_duration",
   "friendly_fire",
   "betrayal_booting",
   "social_flags_bit_2",
   "social_flags_bit_3",
   "social_flags_bit_4",
   "grenades_on_map",
   "indestructible_vehicles",
   "powerup_duration_red",
   "powerup_duration_blue",
   "powerup_duration_yellow",
   "death_event_damage_type",
]);
function var_scope_to_number_scope(vs) {
   switch (vs) {
      case var_scope.global: return number_var_scope.global_number;
      case var_scope.object: return number_var_scope.object_number;
      case var_scope.player: return number_var_scope.player_number;
      case var_scope.team:   return number_var_scope.team_number;
   }
   return null;
}
//
const object_var_scope = _make_enum([
   "global",
   "player",
   "object",
   "team",
   "global_player_biped", // KSoft.Tool calls these "player_slave"
   "player_player_biped",
   "object_player_biped",
   "team_player_biped",
]);
function var_scope_to_object_scope(vs) {
   switch (vs) {
      case var_scope.global: return object_var_scope.global;
      case var_scope.object: return object_var_scope.object;
      case var_scope.player: return object_var_scope.player;
      case var_scope.team:   return object_var_scope.team;
   }
   return null;
}
function var_scope_to_biped_scope(vs) {
   switch (vs) {
      case var_scope.global: return object_var_scope.global_player_biped;
      case var_scope.object: return object_var_scope.object_player_biped;
      case var_scope.player: return object_var_scope.player_player_biped;
      case var_scope.team:   return object_var_scope.team_player_biped;
   }
   return null;
}
//
const team_var_scope = _make_enum([
   "global",
   "player",
   "object",
   "team",
   "object_owner_team",
   "player_owner_team",
]);
function var_scope_to_team_scope(vs) {
   switch (vs) {
      case var_scope.global: return team_var_scope.global;
      case var_scope.object: return team_var_scope.object;
      case var_scope.player: return team_var_scope.player;
      case var_scope.team:   return team_var_scope.team;
   }
   return null;
}
//
const timer_var_scope = _make_enum([
   "global",
   "player",
   "team",
   "object",
   "round_timer",
   "sudden_death_timer",
   "grace_period_timer",
]);

// currently in variables_and_scopes.cpp. need to add real enums alongside these -- messy. :(
const megalo_objects = _make_enum([
   "no_object",
   "global_object_0",
   "global_object_1",
   "global_object_2",
   "global_object_3",
   "global_object_4",
   "global_object_5",
   "global_object_6",
   "global_object_7",
   "global_object_8",
   "global_object_9",
   "global_object_10",
   "global_object_11",
   "global_object_12",
   "global_object_13",
   "global_object_14",
   "global_object_15",
   "current_object",
   "hud_target_object",
   "killed_object",
   "killer_object",
]);
const megalo_players = _make_enum([
   "no_player",
   "player_1",
   "player_2",
   "player_3",
   "player_4",
   "player_5",
   "player_6",
   "player_7",
   "player_8",
   "player_9",
   "player_10",
   "player_11",
   "player_12",
   "player_13",
   "player_14",
   "player_15",
   "player_16",
   "global_player_0",
   "global_player_1",
   "global_player_2",
   "global_player_3",
   "global_player_4",
   "global_player_5",
   "global_player_6",
   "global_player_7",
   "current_player",
   "hud_player",
   "hud_target_player",
   "killer_player",
]);
const megalo_teams = _make_enum([
   "no_team",
   "team_1",
   "team_2",
   "team_3",
   "team_4",
   "team_5",
   "team_6",
   "team_7",
   "team_8",
   "neutral_team",
   "global_team_0",
   "global_team_1",
   "global_team_2",
   "global_team_3",
   "global_team_4",
   "global_team_5",
   "global_team_6",
   "global_team_7",
   "current_team",
   "hud_player.team",
   "hud_target.team",
   "unk_14_team",
   "unk_15_team",
]);

class MScriptProperty {
   constructor(details) {
      this.name              = details.name;
      this.type              = details.type;
      this.allow_from_nested = details.allow_from_nested; // allow (namespace.var.var.property)?
      this.hard_max_index    = details.hard_max_index || 0; // same as on members
      this.is_read_only      = details.is_read_only   || false;
      this.megalo_number     = details.megalo_number || null; // REQUIRED for numbers; simplifies things for types.number.resolve_function
      this.megalo_timer      = details.megalo_timer  || null; // REQUIRED for timers;  simplifies things for types.timer.resolve_function
   }
}
class MScriptTypename {
   constructor(details) {
      this.name              = details.name;
      this.is_nestable       = details.is_nestable; // can this type have other variables on it?
      this.is_static         = details.is_static;   // can this type be indexed directly, e.g. player[1]?
      this.is_variable       = details.is_variable;
      this.always_read_only  = details.always_read_only || false;
      this.first_static_enum = 0; // TODO: C++ would have this be the enum value for team_1, etc., as an int
      this.static_limit      = details.static_limit || 0; // if > 0, then the limit is constant
      this.properties        = details.properties || [];
      this.underlying_type   = details.underlying_type || null;
      this.resolve_function  = details.resolve_function || null;
   }
}
const types = new Collection({
   number: new MScriptTypename({
      name:        "number",
      is_nestable: false,
      is_static:   false,
      is_variable: true,
      resolve_function: function(ref) {
         //
         // POSSIBLE SCOPES:
         // constant           | <any integer constant>
         // global             | global.number[x]
         // player             | global.player[x].number[y] OR unnamed_ns_player_member.number[y]
         // object             | global.object[x].number[y] OR unnamed_ns_object_member.number[y]
         // team               | global.team[x].number[y]   OR unnamed_ns_team_member.number[y]
         // ...
         //
         let last  = ref.parts[ref.parts.length - 1];
         let lObj  = last.analyzed.object;
         if (lObj instanceof MScriptNamespaceMember) {
            assert(lObj.megalo_number !== null, "Unknown namespace member.");
            ref.analyzed.scope = lObj.megalo_number;
            ref.analyzed.index = last.index;
            return;
         }
         let root  = ref.identify_root(); // identifies any of: member; static; namespace.member; namespace.var
         let index = root.part_count;
         if (!index || root.id === null) { // either global.number[n] or unidentifiable
            let part = ref.parts[0];
            let pObj = part.analyzed.object;
            if (pObj == namespaces.global) {
               ref.analyzed.scope = number_var_scope.global_number;
               ref.analyzed.index = ref.parts[1].index;
               return;
            }
            assert(false, "Unknown root.");
         }
         assert(ref.parts.length > index,      "Shouldn't be possible for a number to be a root.");
         assert(index == ref.parts.length - 1, "Unexpected content: root.unknown.last.");
         switch (root.type) {
            case types.object: ref.analyzed.scope = number_var_scope.object_number; break;
            case types.player: ref.analyzed.scope = number_var_scope.player_number; break;
            case types.team:   ref.analyzed.scope = number_var_scope.team_number;   break;
         }
         ref.analyzed.which = root.id;
         ref.analyzed.index = last.index;
         if (lObj instanceof MScriptProperty) {
            assert(lObj.megalo_number !== null, "Unknown number-property.");
            ref.analyzed.scope = lObj.megalo_number;
            if (!last.has_index()) { // object[n].spawn_sequence: no which; index == n; if there is only one index e.g. var[n].prop rather than var[x].prop[y] then (index) is that index
               ref.analyzed.which = null;
               ref.analyzed.index = root.id;
            }
         }
      },
   }),
   object: new MScriptTypename({
      name:        "object",
      is_nestable: true,
      is_static:   false,
      is_variable: true,
      resolve_function: function(ref) {
         //
         // POSSIBLE SCOPES:
         // global              | global.object[x]                 OR unnamed_ns_object_member
         // player              | global.player[x].object[y]       OR unnamed_ns_player_member.object[y]
         // object              | global.object[x].object[y]       OR unnamed_ns_object_member.object[y]
         // team                | global.team[x].object[y]         OR unnamed_ns_team_member.object[y]
         // player_biped        | global.player[x].biped           OR unnamed_ns_player_member.biped
         // player_player_biped | global.player[x].player[y].biped OR unnamed_ns_player_member.player[y].biped
         // object_player_biped | global.object[x].player[y].biped OR unnamed_ns_object_member.player[y].biped
         // team_player_biped   | global.team[x].player[y].biped   OR unnamed_ns_team_member.player[y].biped
         //
         let last  = ref.parts[ref.parts.length - 1];
         let lObj  = last.analyzed.object;
         let root  = ref.identify_root(); // identifies any of: member; static; namespace.member; namespace.var
         let index = root.part_count;
         assert(index && root.id !== null, "Unknown root. Should have been any of: unnamed_ns_object_member; global.var[n].");
         if (ref.parts.length == index) { // just the root
            ref.analyzed.scope = object_var_scope.global;
            ref.analyzed.index = root.id;
            return;
         }
         if (lObj instanceof MScriptProperty && lObj.name == "biped") {
            if (index == ref.parts.length - 1) { // ...player.biped
               ref.analyzed.scope = object_var_scope.global_player_biped;
               ref.analyzed.which = root.id;
            } else { // ...var.player.biped
               let nested = ref.parts[index];
               switch (root.type) {
                  case types.object: ref.analyzed.scope = object_var_scope.object_player_biped; break;
                  case types.player: ref.analyzed.scope = object_var_scope.player_player_biped; break;
                  case types.team:   ref.analyzed.scope = object_var_scope.team_player_biped;   break;
               }
               ref.analyzed.which = root.id;
               ref.analyzed.index = nested.index;
            }
            return;
         }
         switch (root.type) {
            case types.object: ref.analyzed.scope = object_var_scope.object; break;
            case types.player: ref.analyzed.scope = object_var_scope.player; break;
            case types.team:   ref.analyzed.scope = object_var_scope.team;   break;
         }
         let nested = ref.parts[index];
         ref.analyzed.which = root.id;
         ref.analyzed.index = nested.index;
         return;
      },
   }),
   player: new MScriptTypename({
      name:         "player",
      is_nestable:  true,
      is_static:    true,
      is_variable:  true,
      static_limit: 16,
      resolve_function: function(ref) {
         //
         // POSSIBLE SCOPES:
         // global              | global.object[x]                 OR unnamed_ns_player_member OR static
         // player              | global.player[x].player[y]       OR unnamed_ns_player_member.player[y]
         // object              | global.object[x].player[y]       OR unnamed_ns_object_member.player[y]
         // team                | global.team[x].player[y]         OR unnamed_ns_team_member.player[y]
         //
         let root  = ref.identify_root(); // identifies any of: member; static; namespace.member; namespace.var
         let index = root.part_count;
         assert(index && root.id !== null, "Unknown root. Should have been any of: unnamed_ns_object_member; global.var[n].", [ref, root]);
         if (ref.parts.length == index) { // just the root
            ref.analyzed.scope = var_scope.global; // NOTE: in C++, player var scopes have their own enum; same members as the general scope enum, but different order
            ref.analyzed.index = root.id;
            return;
         }
         switch (root.type) {
            case types.object: ref.analyzed.scope = var_scope.object; break; // NOTE: in C++, player var scopes have their own enum; same members as the general scope enum, but different order
            case types.player: ref.analyzed.scope = var_scope.player; break; // NOTE: in C++, player var scopes have their own enum; same members as the general scope enum, but different order
            case types.team:   ref.analyzed.scope = var_scope.team;   break; // NOTE: in C++, player var scopes have their own enum; same members as the general scope enum, but different order
         }
         let nested = ref.parts[index];
         ref.analyzed.which = root.id;
         ref.analyzed.index = nested.index;
         return;
      },
   }),
   team: new MScriptTypename({
      name:         "team",
      is_nestable:  true,
      is_static:    true,
      is_variable:  true,
      static_limit: 8,
      resolve_function: function(ref) {
         //
         // POSSIBLE SCOPES:
         // global              | global.team[x]           OR unnamed_ns_object_member OR static
         // player              | global.player[x].team[y] OR unnamed_ns_player_member.team[y]
         // object              | global.object[x].team[y] OR unnamed_ns_object_member.team[y]
         // team                | global.team[x].team[y]   OR unnamed_ns_team_member.team[y]
         // object_owner_team   | global.object[x].team    OR unnamed_ns_object_member.team
         // player_owner_team   | global.player[x].team    OR unnamed_ns_player_member.team
         //
         let last  = ref.parts[ref.parts.length - 1].analyzed.object;
         let root  = ref.identify_root(); // identifies any of: member; static; namespace.member; namespace.var
         let index = root.part_count;
         assert(index && root.id !== null, "Unknown root.");
         if (ref.parts.length == index) { // just the root
            ref.analyzed.scope = team_var_scope.global;
            ref.analyzed.index = root.id;
            return;
         }
         if (last instanceof MScriptProperty && last.name == "team") {
            switch (root.type) {
               case types.object: ref.analyzed.scope = team_var_scope.object_owner_team; break;
               case types.player: ref.analyzed.scope = team_var_scope.player_owner_team; break;
            }
            ref.analyzed.which = root.id;
            return;
         }
         switch (root.type) {
            case types.object: ref.analyzed.scope = team_var_scope.object; break;
            case types.player: ref.analyzed.scope = team_var_scope.player; break;
            case types.team:   ref.analyzed.scope = team_var_scope.team;   break;
         }
         let nested = ref.parts[index];
         ref.analyzed.which = root.id;
         ref.analyzed.index = nested.index;
         return;
      },
   }),
   timer: new MScriptTypename({
      name:        "timer",
      is_nestable: false,
      is_static:   false,
      is_variable: true,
      resolve_function: function(ref) {
         //
         // POSSIBLE SCOPES:
         // global             | global.timer[x]
         // player             | global.player[x].timer[y] OR unnamed_ns_player_member.timer[y]
         // object             | global.object[x].timer[y] OR unnamed_ns_object_member.timer[y]
         // team               | global.team[x].timer[y]   OR unnamed_ns_team_member.timer[y]
         // round_timer        | game.round_timer
         // sudden_death_timer | game.sudden_death_timer
         // grace_period_timer | game.grace_period_timer
         //
         let last  = ref.parts[ref.parts.length - 1].analyzed.object;
         if (last instanceof MScriptNamespaceMember) {
            assert(last.megalo_timer !== null, "Unknown namespace member.");
            ref.analyzed.scope = last.megalo_timer;
            return;
         }
         let root  = ref.identify_root(); // identifies any of: member; static; namespace.member; namespace.var
         let index = root.part_count;
         if (!index || root.id === null) { // either global.timer[n] or unidentifiable
            let part = ref.parts[0];
            let pObj = part.analyzed.object;
            if (pObj == namespaces.global) {
               ref.analyzed.scope = timer_var_scope.global;
               ref.analyzed.index = ref.parts[1].index;
               return;
            }
            assert(false, "Unknown root.");
         }
         assert(ref.parts.length != index, "Shouldn't be possible for a timer to be a root.");
         switch (root.type) {
            case types.object: ref.analyzed.scope = timer_var_scope.object; break;
            case types.player: ref.analyzed.scope = timer_var_scope.player; break;
            case types.team:   ref.analyzed.scope = timer_var_scope.team;   break;
         }
         let nested = ref.parts[index];
         ref.analyzed.which = root.id;
         ref.analyzed.index = nested.index;
      },
   }),
   //
   script_option: new MScriptTypename({ // unique type for an unnamed-namespace member
      name:             "script_option",
      is_nestable:      false,
      is_static:        false,
      is_variable:      false,
      always_read_only: true,
   }),
});
// in C++, the header/CPP split means that we could define these property lists in the constructors above; the properties could refer to each type because the types would already be declared
types.object.properties = [
   new MScriptProperty({
      name: "spawn_sequence",
      type: types.number,
      megalo_number: number_var_scope.spawn_sequence,
   }),
   new MScriptProperty({ name: "team",           type: types.team   }),
];
types.player.properties = [
   new MScriptProperty({ name: "biped",       type: types.object, allow_from_nested: true }),
   new MScriptProperty({
      name: "rating",
      type: types.number,
      megalo_number: number_var_scope.player_rating,
   }),
   new MScriptProperty({
      name: "score",
      type: types.number,
      megalo_number: number_var_scope.player_score,
   }),
   new MScriptProperty({
      name: "script_stat",
      type: types.number,
      hard_max_index: 3,
      megalo_number: number_var_scope.player_stat,
   }),
   new MScriptProperty({ name: "team",        type: types.team   }),
   new MScriptProperty({
      name: "unknown_09",
      type: types.number,
      megalo_number: number_var_scope.unknown_09,
   }),
];
types.team.properties = [
   new MScriptProperty({
      name: "score",
      type: types.number,
      megalo_number: number_var_scope.team_score,
   }),
   new MScriptProperty({
      name: "script_stat",
      type: types.number,
      hard_max_index: 3,
      megalo_number: number_var_scope.team_stat,
   }),
];
types.script_option.underlying_type = types.number;

class MScriptNamespaceMember {
   constructor(details) {
      this.name         = details.name;
      this.type         = details.type; // MScriptTypename
      this.is_read_only = details.is_read_only;
      this.is_none      = details.is_none || false;
      this.soft_max_index = details.soft_max_index || null; // callback; for script options, this would check how many options the gametype defines
      this.hard_max_index = details.hard_max_index || 0;    // integer maximum; if 0, then no index allowed
      this.megalo_object  = details.megalo_object  || null; // value from the respective array  // REQUIRED for objects; simplifies things for types.object.resolve_function
      this.megalo_player  = details.megalo_player  || null; // value from the respective array  // REQUIRED for players; simplifies things for types.player.resolve_function
      this.megalo_team    = details.megalo_team    || null; // value from the respective array  // REQUIRED for teams;   simplifies things for types.team.resolve_function
      this.megalo_number  = details.megalo_number  || null; // value from the number var scopes // REQUIRED for numbers; simplifies things for types.number.resolve_function
      this.megalo_timer   = details.megalo_timer   || null; // value from the timer  var scopes // REQUIRED for timers;  simplifies things for types.timer.resolve_function
      this.owner = null; // set by containing MScriptNamespace
   }
}
class MScriptNamespace {
   constructor(details) {
      this.name               = details.name;
      this.can_have_statics   = details.can_have_statics;   // only true for unnamed namespace
      this.can_have_variables = details.can_have_variables; // only true for global namespace
      this.members = details.members || [];
      //
      for(let member of this.members)
         member.owner = this;
   }
}

const namespaces = new Collection({
   unnamed: new MScriptNamespace({
      name: "",
      can_have_statics:   true,
      can_have_variables: false,
      members: [
         new MScriptNamespaceMember({ name: "no_object", type: types.object, megalo_object: megalo_objects.no_object, is_read_only: true, is_none: true }),
         new MScriptNamespaceMember({ name: "no_player", type: types.player, megalo_player: megalo_players.no_player, is_read_only: true, is_none: true }),
         new MScriptNamespaceMember({ name: "no_team",   type: types.team,   megalo_team:   megalo_teams.no_team,     is_read_only: true, is_none: true }),
         new MScriptNamespaceMember({ name: "current_object", type: types.object, megalo_player: megalo_players.current_object, is_read_only: true, is_none: false }),
         new MScriptNamespaceMember({ name: "current_player", type: types.player, megalo_player: megalo_players.current_player, is_read_only: true, is_none: false }),
         new MScriptNamespaceMember({ name: "current_team",   type: types.team,   megalo_team:   megalo_teams.current_team,     is_read_only: true, is_none: false }),
         new MScriptNamespaceMember({ name: "neutral_team",   type: types.team,   megalo_team:   megalo_teams.neutral_team,     is_read_only: true, is_none: false }),
         new MScriptNamespaceMember({ name: "hud_target_object", type: types.object, megalo_object: megalo_objects.hud_target_object, is_read_only: true, is_none: false }),
         new MScriptNamespaceMember({ name: "hud_target_player", type: types.player, megalo_player: megalo_players.hud_target_player, is_read_only: true, is_none: false }),
         new MScriptNamespaceMember({ name: "hud_player", type: types.player, megalo_player: megalo_players.hud_player, is_read_only: true, is_none: false }),
         new MScriptNamespaceMember({ name: "killed_object", type: types.object, megalo_object: megalo_objects.killed_object, is_read_only: true, is_none: false }),
         new MScriptNamespaceMember({ name: "killer_object", type: types.object, megalo_object: megalo_objects.killer_object, is_read_only: true, is_none: false }),
         new MScriptNamespaceMember({ name: "killer_player", type: types.player, megalo_player: megalo_players.killer_player, is_read_only: true, is_none: false }),
         new MScriptNamespaceMember({ name: "unk_14_team", type: types.team, megalo_team: megalo_teams.unk_14_team, is_read_only: true, is_none: false }),
         new MScriptNamespaceMember({ name: "unk_15_team", type: types.team, megalo_team: megalo_teams.unk_15_team, is_read_only: true, is_none: false }),
         new MScriptNamespaceMember({
            name:           "script_option",
            type:           types.script_option,
            megalo_number:  number_var_scope.script_option,
            soft_max_index: function(v) { return true; },
            hard_max_index: 15,
         }),
         //
         // TODO: Do we want to add script_traits here?
         //
      ]
   }),
   global: new MScriptNamespace({
      name:               "global",
      can_have_statics:   false,
      can_have_variables: true,
   }),
   game: new MScriptNamespace({
      name:               "game",
      can_have_statics:   false,
      can_have_variables: false,
      members: [
         new MScriptNamespaceMember({ name: "betrayal_booting",        megalo_number: number_var_scope.betrayal_booting,        type: types.number, is_read_only: true, is_none: false }),
         new MScriptNamespaceMember({ name: "betrayal_penalty",        megalo_number: number_var_scope.betrayal_penalty,        type: types.number, is_read_only: true, is_none: false }),
         new MScriptNamespaceMember({ name: "current_round",           megalo_number: number_var_scope.current_round,           type: types.number, is_read_only: true, is_none: false }),
         new MScriptNamespaceMember({ name: "death_event_damage_type", megalo_number: number_var_scope.death_event_damage_type, type: types.number, is_read_only: true, is_none: false }),
         new MScriptNamespaceMember({ name: "friendly_fire",           megalo_number: number_var_scope.friendly_fire,           type: types.number, is_read_only: true, is_none: false }),
         new MScriptNamespaceMember({ name: "grace_period",            megalo_number: number_var_scope.grace_period,            type: types.number, is_read_only: true, is_none: false }),
         new MScriptNamespaceMember({ name: "grenades_on_map",         megalo_number: number_var_scope.grenades_on_map,         type: types.number, is_read_only: true, is_none: false }),
         new MScriptNamespaceMember({ name: "indestructible_vehicles", megalo_number: number_var_scope.indestructible_vehicles, type: types.number, is_read_only: true, is_none: false }),
         new MScriptNamespaceMember({ name: "lives_per_round",         megalo_number: number_var_scope.lives_per_round,         type: types.number, is_read_only: true, is_none: false }),
         new MScriptNamespaceMember({ name: "loadout_cam_time",        megalo_number: number_var_scope.loadout_cam_time,        type: types.number, is_read_only: true, is_none: false }),
         new MScriptNamespaceMember({ name: "misc_unk0_bit3",          megalo_number: number_var_scope.misc_unk0_bit3,          type: types.number, is_read_only: true, is_none: false }),
         new MScriptNamespaceMember({ name: "powerup_duration_red",    megalo_number: number_var_scope.powerup_duration_red,    type: types.number, is_read_only: true, is_none: false }),
         new MScriptNamespaceMember({ name: "powerup_duration_blue",   megalo_number: number_var_scope.powerup_duration_blue,   type: types.number, is_read_only: true, is_none: false }),
         new MScriptNamespaceMember({ name: "powerup_duration_yellow", megalo_number: number_var_scope.powerup_duration_yellow, type: types.number, is_read_only: true, is_none: false }),
         new MScriptNamespaceMember({ name: "respawn_growth",          megalo_number: number_var_scope.respawn_growth,          type: types.number, is_read_only: true, is_none: false }),
         new MScriptNamespaceMember({ name: "respawn_time",            megalo_number: number_var_scope.respawn_time,            type: types.number, is_read_only: true, is_none: false }),
         new MScriptNamespaceMember({ name: "respawn_traits_duration", megalo_number: number_var_scope.respawn_traits_duration, type: types.number, is_read_only: true, is_none: false }),
         new MScriptNamespaceMember({ name: "round_limit",             megalo_number: number_var_scope.round_limit,             type: types.number, is_read_only: true, is_none: false }),
         new MScriptNamespaceMember({ name: "round_time_limit",        megalo_number: number_var_scope.round_time_limit,        type: types.number, is_read_only: true, is_none: false }),
         new MScriptNamespaceMember({ name: "rounds_to_win",           megalo_number: number_var_scope.rounds_to_win,           type: types.number, is_read_only: true, is_none: false }),
         new MScriptNamespaceMember({ name: "score_to_win",            megalo_number: number_var_scope.score_to_win,            type: types.number, is_read_only: true, is_none: false }),
         new MScriptNamespaceMember({ name: "social_flags_bit_2",      megalo_number: number_var_scope.social_flags_bit_2,      type: types.number, is_read_only: true, is_none: false }),
         new MScriptNamespaceMember({ name: "social_flags_bit_3",      megalo_number: number_var_scope.social_flags_bit_3,      type: types.number, is_read_only: true, is_none: false }),
         new MScriptNamespaceMember({ name: "social_flags_bit_4",      megalo_number: number_var_scope.social_flags_bit_4,      type: types.number, is_read_only: true, is_none: false }),
         new MScriptNamespaceMember({ name: "sudden_death_time",       megalo_number: number_var_scope.sudden_death_time,       type: types.number, is_read_only: true, is_none: false }),
         new MScriptNamespaceMember({ name: "suicide_penalty",         megalo_number: number_var_scope.suicide_penalty,         type: types.number, is_read_only: true, is_none: false }),
         new MScriptNamespaceMember({ name: "symmetry",                megalo_number: number_var_scope.symmetry,                type: types.number, is_read_only: false, is_none: false }),
         new MScriptNamespaceMember({ name: "symmetry_getter",         megalo_number: number_var_scope.symmetry_getter,         type: types.number, is_read_only: true, is_none: false }),
         new MScriptNamespaceMember({ name: "team_lives_per_round",    megalo_number: number_var_scope.team_lives_per_round,    type: types.number, is_read_only: true, is_none: false }),
         new MScriptNamespaceMember({ name: "teams_enabled",           megalo_number: number_var_scope.teams_enabled,           type: types.number, is_read_only: true, is_none: false }),
         new MScriptNamespaceMember({ name: "unkF7A6",                 megalo_number: number_var_scope.unkF7A6,                 type: types.number, is_read_only: true, is_none: false }),
         //
         new MScriptNamespaceMember({ name: "round_timer",        megalo_timer: timer_var_scope.round_timer,        type: types.timer, is_read_only: true, is_none: false }),
         new MScriptNamespaceMember({ name: "sudden_death_timer", megalo_timer: timer_var_scope.sudden_death_timer, type: types.timer, is_read_only: true, is_none: false }),
         new MScriptNamespaceMember({ name: "grace_period_timer", megalo_timer: timer_var_scope.grace_period_timer, type: types.timer, is_read_only: true, is_none: false }),
      ]
   }),
});