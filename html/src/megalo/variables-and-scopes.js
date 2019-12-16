const megalo_scope = {
   not_applicable: -1,
   global: 0,
   player: 1,
   object: 2,
   team:   3,
};
const megalo_variable_type = {
   numeric: 0,
   timer:   1,
   team:    2,
   player:  3,
   object:  4,
};

const megalo_objects = (function() {
   let r = [ "none" ];
   for(let i = 0; i < 16; i++)
      r.push("global_object_" + i);
   r = r.concat([
      "current_object",
      "hud_target",
      "killed",
      "killer",
   ]);
   return r;
})();
const megalo_players = (function() {
   let r = [ "none" ];
   for(let i = 0; i < 16; i++)
      r.push("player_player_" + i);
   for(let i = 0; i < 8; i++)
      r.push("global_player_" + i);
   r = r.concat([
      "current_player",
      "hud_player",
      "hud_target",
      "killer",
   ]);
   return r;
})();
const megalo_teams = (function() {
   // NOTE: (none) is -1; the entire enum 
   // is incremented when serialized
   //
   // TODO: wait, though, why do we care? 
   // we can hold these values in memory 
   // however we want, no?
   let r = [ "none" ];
   for(let i = 0; i < 8; i++)
      r.push("Team " + (i + 1));
   r.push("Neutral");
   for(let i = 0; i < 8; i++)
      r.push("global_team_" + i);
   r = r.concat([
      "current_team",
      "hud_player",
      "hud_target",
      "unk_14",
      "unk_15",
   ]);
   return r;
})();


class MegaloVariableDefinition {
   //
   // TODO: use for variable declarations
   //
}

class MegaloVariableScope {
   constructor(list, n, i, e, p, o) {
      this.list = list; // null, for globals
      this.max_numbers = n;
      this.max_timers  = i;
      this.max_teams   = e;
      this.max_players = p;
      this.max_objects = o;
   }
   /*constexpr const int*/ which_bits() /*const noexcept*/ {
      if (!this.list || !this.list.length)
         return 0;
      return _bitcount(this.list.length - 1);
   }
   /*constexpr const int*/ index_bits(variable_type) /*const noexcept*/ {
      let max = this["max_" + variable_type + "s"];
      return _bitcount(max - 1);
   }
}

MegaloVariableScopeGlobal = new MegaloVariableScope(null, 12, 8, 8, 8, 16);
MegaloVariableScopePlayer = new MegaloVariableScope(megalo_players,  8, 4, 4, 4,  4);
MegaloVariableScopeObject = new MegaloVariableScope(megalo_objects,  8, 4, 2, 4,  4);
MegaloVariableScopeTeam   = new MegaloVariableScope(megalo_teams,    8, 4, 4, 4,  6);