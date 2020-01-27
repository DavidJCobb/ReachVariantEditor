class MScriptTypename {
   constructor(details) {
      this.name              = details.name;
      this.is_nestable       = details.is_nestable; // can this type have other variables on it?
      this.is_static         = details.is_static;   // can this type be indexed directly, e.g. player[1]?
      this.is_variable       = details.is_variable;
      this.always_read_only  = details.always_read_only || false;
      this.first_static_enum = 0; // TODO: C++ would have this be the enum value for team_1, etc., as an int
      this.static_limit      = details.static_limit || 0; // if > 0, then the limit is constant
      this.properties        = [];
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
         new MScriptNamespaceMember({
            name:           "script_option",
            type:           types.script_option,
            soft_max_index: function(v) { return true; },
            hard_max_index: 15,
         }),
         //
         // TODO
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
         new MScriptNamespaceMember({ name: "round_time_limit", type: types.number, is_read_only: true, is_none: false }),
         //
         // TODO
         //
      ]
   }),
});