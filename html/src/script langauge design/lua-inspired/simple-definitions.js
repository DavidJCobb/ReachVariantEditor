class MScriptProperty {
   constructor(details) {
      this.name              = details.name;
      this.type              = details.type;
      this.allow_from_nested = details.allow_from_nested; // allow (namespace.var.var.property)?
      this.hard_max_index    = details.hard_max_index || 0; // same as on members
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
   }
}
const types = new Collection({
   number: new MScriptTypename({
      name:        "number",
      is_nestable: false,
      is_static:   false,
      is_variable: true,
   }),
   object: new MScriptTypename({
      name:        "object",
      is_nestable: true,
      is_static:   false,
      is_variable: true,
   }),
   player: new MScriptTypename({
      name:         "player",
      is_nestable:  true,
      is_static:    true,
      is_variable:  true,
      static_limit: 16,
   }),
   team: new MScriptTypename({
      name:         "team",
      is_nestable:  true,
      is_static:    true,
      is_variable:  true,
      static_limit: 8,
   }),
   timer: new MScriptTypename({
      name:        "timer",
      is_nestable: false,
      is_static:   false,
      is_variable: true,
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
   new MScriptProperty({ name: "spawn_sequence", type: types.number }),
   new MScriptProperty({ name: "team",           type: types.team   }),
];
types.player.properties = [
   new MScriptProperty({ name: "biped",       type: types.object, allow_from_nested: true }),
   new MScriptProperty({ name: "rating",      type: types.number }),
   new MScriptProperty({ name: "score",       type: types.number }),
   new MScriptProperty({ name: "script_stat", type: types.number, hard_max_index: 3 }),
   new MScriptProperty({ name: "team",        type: types.team   }),
   new MScriptProperty({ name: "unknown_09",  type: types.number }),
];
types.team.properties = [
   new MScriptProperty({ name: "score",       type: types.number }),
   new MScriptProperty({ name: "script_stat", type: types.number, hard_max_index: 3 }),
];

class MScriptNamespaceMember {
   constructor(details) {
      this.name         = details.name;
      this.type         = details.type; // MScriptTypename
      this.is_read_only = details.is_read_only;
      this.is_none      = details.is_none || false;
      this.soft_max_index = details.soft_max_index || null; // callback; for script options, this would check how many options the gametype defines
      this.hard_max_index = details.hard_max_index || 0;    // integer maximum; if 0, then no index allowed
      //
      // TODO: which/index fields, for things like distinguishing "no_team" from "neutral_team"
      //
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
         new MScriptNamespaceMember({ name: "no_object", type: types.object, is_read_only: true, is_none: true }),
         new MScriptNamespaceMember({ name: "no_player", type: types.player, is_read_only: true, is_none: true }),
         new MScriptNamespaceMember({ name: "no_team",   type: types.team,   is_read_only: true, is_none: true }),
         new MScriptNamespaceMember({ name: "current_object", type: types.object, is_read_only: true, is_none: false }),
         new MScriptNamespaceMember({ name: "current_player", type: types.player, is_read_only: true, is_none: false }),
         new MScriptNamespaceMember({ name: "current_team",   type: types.team,   is_read_only: true, is_none: false }),
         new MScriptNamespaceMember({ name: "neutral_team",   type: types.team,   is_read_only: true, is_none: false }),
         new MScriptNamespaceMember({ name: "hud_target_object", type: types.object, is_read_only: true, is_none: false }),
         new MScriptNamespaceMember({ name: "hud_target_player", type: types.player, is_read_only: true, is_none: false }),
         new MScriptNamespaceMember({ name: "hud_player", type: types.player, is_read_only: true, is_none: false }),
         new MScriptNamespaceMember({ name: "killed_object", type: types.object, is_read_only: true, is_none: false }),
         new MScriptNamespaceMember({ name: "killer_object", type: types.object, is_read_only: true, is_none: false }),
         new MScriptNamespaceMember({ name: "killer_player", type: types.player, is_read_only: true, is_none: false }),
         new MScriptNamespaceMember({ name: "unk_14_team", type: types.team, is_read_only: true, is_none: false }),
         new MScriptNamespaceMember({ name: "unk_15_team", type: types.team, is_read_only: true, is_none: false }),
         new MScriptNamespaceMember({
            name:           "script_option",
            type:           types.script_option,
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
         new MScriptNamespaceMember({ name: "betrayal_booting",        type: types.number, is_read_only: true, is_none: false }),
         new MScriptNamespaceMember({ name: "betrayal_penalty",        type: types.number, is_read_only: true, is_none: false }),
         new MScriptNamespaceMember({ name: "current_round",           type: types.number, is_read_only: true, is_none: false }),
         new MScriptNamespaceMember({ name: "death_event_damage_type", type: types.number, is_read_only: true, is_none: false }),
         new MScriptNamespaceMember({ name: "friendly_fire",           type: types.number, is_read_only: true, is_none: false }),
         new MScriptNamespaceMember({ name: "grace_period",            type: types.number, is_read_only: true, is_none: false }),
         new MScriptNamespaceMember({ name: "grenades_on_map",         type: types.number, is_read_only: true, is_none: false }),
         new MScriptNamespaceMember({ name: "indestructible_vehicles", type: types.number, is_read_only: true, is_none: false }),
         new MScriptNamespaceMember({ name: "lives_per_round",         type: types.number, is_read_only: true, is_none: false }),
         new MScriptNamespaceMember({ name: "loadout_cam_time",        type: types.number, is_read_only: true, is_none: false }),
         new MScriptNamespaceMember({ name: "misc_unk0_bit3",          type: types.number, is_read_only: true, is_none: false }),
         new MScriptNamespaceMember({ name: "powerup_duration_red",    type: types.number, is_read_only: true, is_none: false }),
         new MScriptNamespaceMember({ name: "powerup_duration_blue",   type: types.number, is_read_only: true, is_none: false }),
         new MScriptNamespaceMember({ name: "powerup_duration_yellow", type: types.number, is_read_only: true, is_none: false }),
         new MScriptNamespaceMember({ name: "respawn_growth",          type: types.number, is_read_only: true, is_none: false }),
         new MScriptNamespaceMember({ name: "respawn_time",            type: types.number, is_read_only: true, is_none: false }),
         new MScriptNamespaceMember({ name: "respawn_traits_duration", type: types.number, is_read_only: true, is_none: false }),
         new MScriptNamespaceMember({ name: "round_limit",             type: types.number, is_read_only: true, is_none: false }),
         new MScriptNamespaceMember({ name: "round_time_limit",        type: types.number, is_read_only: true, is_none: false }),
         new MScriptNamespaceMember({ name: "rounds_to_win",           type: types.number, is_read_only: true, is_none: false }),
         new MScriptNamespaceMember({ name: "score_to_win",            type: types.number, is_read_only: true, is_none: false }),
         new MScriptNamespaceMember({ name: "social_flags_bit_2",      type: types.number, is_read_only: true, is_none: false }),
         new MScriptNamespaceMember({ name: "social_flags_bit_3",      type: types.number, is_read_only: true, is_none: false }),
         new MScriptNamespaceMember({ name: "social_flags_bit_4",      type: types.number, is_read_only: true, is_none: false }),
         new MScriptNamespaceMember({ name: "sudden_death_time",       type: types.number, is_read_only: true, is_none: false }),
         new MScriptNamespaceMember({ name: "suicide_penalty",         type: types.number, is_read_only: true, is_none: false }),
         new MScriptNamespaceMember({ name: "symmetry",                type: types.number, is_read_only: false, is_none: false }),
         new MScriptNamespaceMember({ name: "symmetry_getter",         type: types.number, is_read_only: true, is_none: false }),
         new MScriptNamespaceMember({ name: "team_lives_per_round",    type: types.number, is_read_only: true, is_none: false }),
         new MScriptNamespaceMember({ name: "teams_enabled",           type: types.number, is_read_only: true, is_none: false }),
         new MScriptNamespaceMember({ name: "unkF7A6",                 type: types.number, is_read_only: true, is_none: false }),
         //
         new MScriptNamespaceMember({ name: "round_timer",        type: types.timer, is_read_only: true, is_none: false }),
         new MScriptNamespaceMember({ name: "sudden_death_timer", type: types.timer, is_read_only: true, is_none: false }),
         new MScriptNamespaceMember({ name: "grace_period_timer", type: types.timer, is_read_only: true, is_none: false }),
      ]
   }),
});