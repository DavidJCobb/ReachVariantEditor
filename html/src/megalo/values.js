class MegaloValue {
   constructor() {
   }
   static /*MegaloValue*/ factory(stream) /*noexcept*/ {
      __purecall();
      return new MegaloValue();
   }
   /*virtual bool*/ read(stream) {
      __purecall();
   }
   /*virtual string*/ toString() {
      __purecall();
   }
}

class MegaloValueBaseEnum {
   constructor(values, offset) {
      this.baseValues = values;
      this.baseOffset = offset || 0;
      this.value      = null;
   }
   /*virtual bool*/ read(stream) {
      this.value = stream.readBits(_bitcount(this.baseValues.length - 1)) - this.baseOffset;
   }
   /*virtual string*/ toString() {
      for(let i = 0; i < this.baseValues.length; i++)
         if (this.value == i)
            return this.baseValues[i];
      return "unknown:" + this.value;
   }
}
//
class MegaloValueAddWeaponEnum extends MegaloValueBaseEnum {
   constructor() {
      super([
         "add",
         "unk_1",
         "swap",
      ]);
   }
   static /*MegaloValue*/ factory(stream) /*noexcept*/ {
      return new MegaloValueAddWeaponEnum();
   }
}
class MegaloValueCHUDDestinationEnum extends MegaloValueBaseEnum {
   constructor() {
      super([
         "unk_0",
         "unk_1",
      ]);
   }
   static /*MegaloValue*/ factory(stream) /*noexcept*/ {
      return new MegaloValueCHUDDestinationEnum();
   }
}
class MegaloValueCompareOperatorEnum extends MegaloValueBaseEnum {
   constructor() {
      super([
         "less than",
         "greater than",
         "equal to",
         "less than or equal to",
         "greater than or equal to",
         "not equal to",
      ]);
   }
   static /*MegaloValue*/ factory(stream) /*noexcept*/ {
      return new MegaloValueCompareOperatorEnum();
   }
}
class MegaloValueDropWeaponEnum extends MegaloValueBaseEnum {
   constructor() {
      super([
         "primary",
         "secondary",
      ]);
   }
   static /*MegaloValue*/ factory(stream) /*noexcept*/ {
      return new MegaloValueDropWeaponEnum();
   }
}
class MegaloValueGrenadeTypeEnum extends MegaloValueBaseEnum {
   constructor() {
      super([
         "frag",
         "plasma",
      ]);
   }
   static /*MegaloValue*/ factory(stream) /*noexcept*/ {
      return new MegaloValueGrenadeTypeEnum();
   }
}
class MegaloValueMathOperatorEnum extends MegaloValueBaseEnum {
   constructor() {
      super([
         "add",
         "subtract",
         "multiply",
         "divide",
         "set to",
         "modulo by",
         "bitwise AND with",
         "bitwise OR with",
         "bitwise XOR with",
         "set to bitwise NOT of",
         "bitshift left by",
         "bitshift right by",
         "arith shift left by", // guessed; it's some kind of left shift
      ]);
   }
   static /*MegaloValue*/ factory(stream) /*noexcept*/ {
      return new MegaloValueMathOperatorEnum();
   }
}
class MegaloValuePickupPriorityEnum extends MegaloValueBaseEnum {
   constructor() {
      super([
         "unk_0",
         "hold action",
         "automatic",
         "unk_3",
      ]);
   }
   static /*MegaloValue*/ factory(stream) /*noexcept*/ {
      return new MegaloValuePickupPriorityEnum();
   }
}
class MegaloValueTeamDispositionEnum extends MegaloValueBaseEnum {
   constructor() {
      super([
         "unk_0",
         "unk_1",
         "unk_2",
      ]);
   }
   static /*MegaloValue*/ factory(stream) /*noexcept*/ {
      return new MegaloValueTeamDispositionEnum();
   }
}

class MegaloValueBaseFlags {
   constructor(flags) {
      this.baseFlags = flags;
      this.value     = null;
   }
   /*virtual bool*/ read(stream) {
      this.value = stream.readBits(this.baseFlags.length);
   }
   /*virtual string*/ toString() {
      let s = "";
      for(let i = 0; i < this.baseFlags.length; i++) {
         if (this.value & (1 << i)) {
            if (s)
               s += ", ";
            s += this.baseFlags[i];
         }
      }
      return s;
   }
}
//
class MegaloValueCreateObjectFlags extends MegaloValueBaseFlags {
   constructor() {
      super([
         "never garbage-collect",
         "unk 1",
         "unk 2",
      ]);
   }
   static /*MegaloValue*/ factory(stream) /*noexcept*/ {
      return new MegaloValueCreateObjectFlags();
   }
}
class MegaloValueKillerTypeFlags extends MegaloValueBaseFlags {
   constructor() {
      super([
         "guardians",
         "suicide",
         "kill",
         "betrayal",
         "quit",
      ]);
   }
   static /*MegaloValue*/ factory(stream) /*noexcept*/ {
      return new MegaloValueKillerTypeFlags();
   }
}
class MegaloValuePlayerUnusedModeFlags extends MegaloValueBaseFlags {
   constructor() {
      super([
         "unk 0",
         "unk 1",
         "unk 2",
         "unk 3",
      ]);
   }
   static /*MegaloValue*/ factory(stream) /*noexcept*/ {
      return new MegaloValuePlayerUnusedModeFlags();
   }
}

const MegaloValueIndexQuirk = {
   none:      0,
   presence:  1,
   reference: 2,
   word:      3,
   offset:    4,
}
class MegaloValueBaseIndex {
   constructor(name, max, quirk) {
      this.baseName  = name;
      this.baseMax   = max;
      this.baseQuirk = quirk || MegaloValueIndexQuirk.none;
      this.value = null;
   }
   /*virtual bool*/ read(stream) {
      if (this.baseQuirk == MegaloValueIndexQuirk.presence) {
         let absence = stream.readBits(1) != 0;
         if (absence) {
            this.value = -1;
            return;
         }
      }
      this.value = stream.readBits(_bitcount(this.baseMax - 1));
      if (this.baseQuirk == MegaloValueIndexQuirk.offset)
         this.value--;
   }
   /*virtual string*/ toString() {
      return this.baseName + " #" + this.value;
   }
}
//
class MegaloValueLabelIndex extends MegaloValueBaseIndex {
   constructor() {
      super("Forge Label", 16, MegaloValueIndexQuirk.presence);
   }
   static /*MegaloValue*/ factory(stream) /*noexcept*/ {
      return new MegaloValueLabelIndex();
   }
}
class MegaloValueMPObjectTypeIndex extends MegaloValueBaseIndex {
   constructor() {
      super("MP Object Type", 2048, MegaloValueIndexQuirk.presence);
   }
   static /*MegaloValue*/ factory(stream) /*noexcept*/ {
      return new MegaloValueMPObjectTypeIndex();
   }
}

function MegaloValueAnyFactory(stream) {
   let type = stream.readBits(3);
   switch (type) {
      case megalo_variable_type.numeric:
         return MegaloValueScalar.factory.call(null, arguments);
      case megalo_variable_type.timer:
         return MegaloValueTimer.factory.call(null, arguments);
      case megalo_variable_type.team:
         return MegaloValueTeam.factory.call(null, arguments);
      case megalo_variable_type.player:
         return MegaloValuePlayer.factory.call(null, arguments);
      case megalo_variable_type.object:
         return MegaloValueObject.factory.call(null, arguments);
      default:
         console.warn("Unrecognized any-value-type " + type + ".");
         return null;
   }
}

const MegaloValueScalarScopes = (function() {
   let values = [
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
   ];
   let r = {};
   for(let i = 0; i < values.length; i++)
      r[values[i]] = i;
   return r;
})();
class MegaloValueScalar extends MegaloValue {
   constructor() {
      super();
      this.scope = null; // e.g. Player.Number, Spawn Sequence Of, etc.
      this.which = null; // e.g. object for spawn sequence
      this.index = null; // e.g. variable index in scope; constant value; etc.
   }
   static /*MegaloValue*/ factory(stream) /*noexcept*/ {
      return new MegaloValueScalar();
   }
   /*bool*/ read(stream) {
      this.scope = stream.readBits(_bitcount(Object.keys(MegaloValueScalarScopes).length - 1));
      let which_bits;
      let index_bits;
      let variable_scope = null;
      switch (this.scope) {
         case MegaloValueScalarScopes.constant:
            this.index = _byteswap_ushort(stream.readSInt16()); // JS needs to byteswap; C++ does not; JS bitstream is probably using the wrong endianness
            break;
         case MegaloValueScalarScopes.player_number:
            variable_scope = MegaloVariableScopePlayer;
            break;
         case MegaloValueScalarScopes.object_number:
            variable_scope = MegaloVariableScopeObject;
            break;
         case MegaloValueScalarScopes.team_number:
            variable_scope = MegaloVariableScopeTeam;
            break;
         case MegaloValueScalarScopes.global_number:
            variable_scope = MegaloVariableScopeGlobal;
            break;
         case MegaloValueScalarScopes.script_option:
            index_bits = _bitcount(16 - 1);
            break;
         case MegaloValueScalarScopes.spawn_sequence:
            which_bits = MegaloVariableScopeObject.which_bits();
            break;
         case MegaloValueScalarScopes.team_score:
            which_bits = MegaloVariableScopeTeam.which_bits();
            break;
         case MegaloValueScalarScopes.player_score:
            which_bits = MegaloVariableScopePlayer.which_bits();
            break;
         case MegaloValueScalarScopes.unknown_09: // unused in Reach? used in Halo 4?
            which_bits = MegaloVariableScopePlayer.which_bits();
            break;
         case MegaloValueScalarScopes.player_rating:
            which_bits = MegaloVariableScopePlayer.which_bits();
            break;
         case MegaloValueScalarScopes.player_stat:
            which_bits = MegaloVariableScopePlayer.which_bits();
            index_bits = _bitcount(4 - 1);
            break;
         case MegaloValueScalarScopes.team_stat:
            which_bits = MegaloVariableScopeTeam.which_bits();
            index_bits = _bitcount(4 - 1);
            break;
         case MegaloValueScalarScopes.current_round:
         case MegaloValueScalarScopes.symmetry_getter:
         case MegaloValueScalarScopes.symmetry:
         case MegaloValueScalarScopes.score_to_win:
         case MegaloValueScalarScopes.unkF7A6:
         case MegaloValueScalarScopes.teams_enabled:
         case MegaloValueScalarScopes.round_time_limit:
         case MegaloValueScalarScopes.round_limit:
         case MegaloValueScalarScopes.misc_unk0_bit3:
         case MegaloValueScalarScopes.rounds_to_win:
         case MegaloValueScalarScopes.sudden_death_time:
         case MegaloValueScalarScopes.grace_period:
         case MegaloValueScalarScopes.lives_per_round:
         case MegaloValueScalarScopes.team_lives_per_round:
         case MegaloValueScalarScopes.respawn_time:
         case MegaloValueScalarScopes.suicide_penalty:
         case MegaloValueScalarScopes.betrayal_penalty:
         case MegaloValueScalarScopes.respawn_growth:
         case MegaloValueScalarScopes.loadout_cam_time:
         case MegaloValueScalarScopes.respawn_traits_duration:
         case MegaloValueScalarScopes.friendly_fire:
         case MegaloValueScalarScopes.betrayal_booting:
         case MegaloValueScalarScopes.social_flags_bit_2:
         case MegaloValueScalarScopes.social_flags_bit_3:
         case MegaloValueScalarScopes.social_flags_bit_4:
         case MegaloValueScalarScopes.grenades_on_map:
         case MegaloValueScalarScopes.indestructible_vehicles:
         case MegaloValueScalarScopes.powerup_duration_red:
         case MegaloValueScalarScopes.powerup_duration_blue:
         case MegaloValueScalarScopes.powerup_duration_yellow:
         case MegaloValueScalarScopes.death_event_damage_type:
            break;
         default:
            console.warn("Scalar value failed to load: unknown scope " + this.scope + ".");
            return false;
      }
      if (variable_scope) {
         //this.scope = variable_scope;
         this.which = stream.readBits(variable_scope.which_bits());
         this.index = stream.readBits(variable_scope.index_bits("number"));
         return true;
      }
      if (which_bits)
         this.which = stream.readBits(which_bits);
      if (index_bits)
         this.index = stream.readBits(index_bits);
      return true;
   }
   toString() {
      switch (this.scope) {
         case MegaloValueScalarScopes.constant:
            return this.index;
         case MegaloValueScalarScopes.player_number:
            return megalo_players[this.which] + ".Number[" + this.index + "]";
         case MegaloValueScalarScopes.object_number:
            return megalo_objects[this.which] + ".Number[" + this.index + "]";
         case MegaloValueScalarScopes.team_number:
            return megalo_teams[this.which] + ".Number[" + this.index + "]";
         case MegaloValueScalarScopes.global_number:
            return "Global.Number[" + this.index + "]";
         case MegaloValueScalarScopes.script_option:
            return "Script Option #" + this.index;
         case MegaloValueScalarScopes.spawn_sequence:
            return "Spawn Sequence of " + megalo_objects[this.which];
         case MegaloValueScalarScopes.team_score:
            return "Score of " + megalo_teams[this.which];
         case MegaloValueScalarScopes.player_score:
            return "Score of " + megalo_players[this.which];
         case MegaloValueScalarScopes.unknown_09:
            return "Unknown-09 of " + megalo_players[this.which];
         case MegaloValueScalarScopes.player_rating:
            return "Rating of " + megalo_players[this.which];
         case MegaloValueScalarScopes.player_stat:
            return "Stat " + this.index + " of " + megalo_players[this.which];
         case MegaloValueScalarScopes.team_stat:
            return "Stat " + this.index + " of " + megalo_teams[this.which];
         case MegaloValueScalarScopes.current_round:
            return "Current Round";
         case MegaloValueScalarScopes.symmetry_getter:
            return "Symmetry (Read-Only)";
         case MegaloValueScalarScopes.symmetry:
            return "Symmetry";
         case MegaloValueScalarScopes.score_to_win:
            return "Score to Win";
         case MegaloValueScalarScopes.unkF7A6:
            return "unkF7A6";
         case MegaloValueScalarScopes.teams_enabled:
            return "Teams Enabled";
         case MegaloValueScalarScopes.round_time_limit:
            return "Round Time Limit";
         case MegaloValueScalarScopes.round_limit:
            return "Round Limit";
         case MegaloValueScalarScopes.misc_unk0_bit3:
            return "MiscOptions:Unk0:Bit3";
         case MegaloValueScalarScopes.rounds_to_win:
            return "Rounds To Win";
         case MegaloValueScalarScopes.sudden_death_time:
            return "Sudden Death Time Limit";
         case MegaloValueScalarScopes.grace_period:
            return "Grace Period";
         case MegaloValueScalarScopes.lives_per_round:
            return "Lives Per Round";
         case MegaloValueScalarScopes.team_lives_per_round:
            return "Team Lives Per Round";
         case MegaloValueScalarScopes.respawn_time:
            return "Respawn Time";
         case MegaloValueScalarScopes.suicide_penalty:
         case MegaloValueScalarScopes.betrayal_penalty:
         case MegaloValueScalarScopes.respawn_growth:
         case MegaloValueScalarScopes.loadout_cam_time:
         case MegaloValueScalarScopes.respawn_traits_duration:
         case MegaloValueScalarScopes.friendly_fire:
         case MegaloValueScalarScopes.betrayal_booting:
         case MegaloValueScalarScopes.social_flags_bit_2:
         case MegaloValueScalarScopes.social_flags_bit_3:
         case MegaloValueScalarScopes.social_flags_bit_4:
         case MegaloValueScalarScopes.grenades_on_map:
         case MegaloValueScalarScopes.indestructible_vehicles:
         case MegaloValueScalarScopes.powerup_duration_red:
         case MegaloValueScalarScopes.powerup_duration_blue:
         case MegaloValueScalarScopes.powerup_duration_yellow:
         case MegaloValueScalarScopes.death_event_damage_type:
            for(let i in MegaloValueScalarScopes) // i'm lazy
               if (MegaloValueScalarScopes[i] == this.scope)
                  return i;
            break;
      }
   }
}

const MegaloValuePlayerScopes = {
   global: 0,
   player: 1,
   object: 2,
   team:   3,
};
class MegaloValuePlayer extends MegaloValue {
   constructor() {
      super();
      this.scope = null; // e.g. Object.Player
      this.which = null; // e.g. WHICH scope e.g. which object/player/team
      this.index = null; // e.g. variable index in scope; constant value; etc.
   }
   static /*MegaloValue*/ factory(stream) /*noexcept*/ {
      return new MegaloValuePlayer();
   }
   /*bool*/ read(stream) {
      this.scope = stream.readBits(_bitcount(Object.keys(MegaloValuePlayerScopes).length - 1));
      let variable_scope = null;
      switch (this.scope) {
         case MegaloValuePlayerScopes.global:
            this.index = stream.readBits(MegaloVariableScopePlayer.which_bits());
            break;
         case MegaloValuePlayerScopes.player:
            variable_scope = MegaloVariableScopePlayer;
            break;
         case MegaloValuePlayerScopes.object:
            variable_scope = MegaloVariableScopeObject;
            break;
         case MegaloValuePlayerScopes.team:
            variable_scope = MegaloVariableScopeTeam;
            break;
         default:
            console.warn("Player value failed to load: unknown scope " + this.scope + ".");
            return false;
      }
      if (variable_scope) {
         //this.scope = variable_scope;
         this.which = stream.readBits(variable_scope.which_bits());
         this.index = stream.readBits(variable_scope.index_bits("player"));
         return true;
      }
      return true;
   }
   toString() {
      switch (this.scope) {
         case MegaloValuePlayerScopes.global:
            return megalo_players[this.index];
         case MegaloValuePlayerScopes.player:
            return megalo_players[this.which] + ".Player[" + this.index + "]";
         case MegaloValuePlayerScopes.object:
            return megalo_objects[this.which] + ".Player[" + this.index + "]";
         case MegaloValuePlayerScopes.team:
            return megalo_teams[this.which] + ".Player[" + this.index + "]";
      }
   }
}

const MegaloValueObjectScopes = {
   global: 0,
   player: 1,
   object: 2,
   team:   3,
   player_slave:        4,
   player_player_slave: 5,
   object_player_slave: 6,
   team_player_slave:   7,
};
class MegaloValueObject extends MegaloValue {
   constructor() {
      super();
      this.scope = null; // e.g. Object.Object
      this.which = null; // e.g. WHICH scope e.g. which object/player/team
      this.index = null; // e.g. variable index in scope; constant value; etc.
   }
   static /*MegaloValue*/ factory(stream) /*noexcept*/ {
      return new MegaloValueObject();
   }
   /*bool*/ read(stream) {
      this.scope = stream.readBits(_bitcount(Object.keys(MegaloValueObjectScopes).length - 1));
      let variable_scope = null;
      switch (this.scope) {
         case MegaloValueObjectScopes.global:
            this.index = stream.readBits(MegaloVariableScopeObject.which_bits());
            break;
         case MegaloValueObjectScopes.player:
            variable_scope = MegaloVariableScopePlayer;
            break;
         case MegaloValueObjectScopes.object:
            variable_scope = MegaloVariableScopeObject;
            break;
         case MegaloValueObjectScopes.team:
            variable_scope = MegaloVariableScopeTeam;
            break;
         case MegaloValueObjectScopes.player_slave:
            this.which = stream.readBits(MegaloVariableScopePlayer.which_bits());
            break;
         case MegaloValueObjectScopes.player_player_slave:
            this.which = stream.readBits(MegaloVariableScopePlayer.which_bits());
            this.index = stream.readBits(MegaloVariableScopePlayer.index_bits("player"));
            break;
         case MegaloValueObjectScopes.object_player_slave:
            this.which = stream.readBits(MegaloVariableScopeObject.which_bits());
            this.index = stream.readBits(MegaloVariableScopeObject.index_bits("player"));
            break;
         case MegaloValueObjectScopes.team_player_slave:
            this.which = stream.readBits(MegaloVariableScopeTeam.which_bits());
            this.index = stream.readBits(MegaloVariableScopeTeam.index_bits("player"));
            break;
         default:
            console.warn("Object value failed to load: unknown scope " + this.scope + ".");
            return false;
      }
      if (variable_scope) {
         //this.scope = variable_scope;
         this.which = stream.readBits(variable_scope.which_bits());
         this.index = stream.readBits(variable_scope.index_bits("object"));
         return true;
      }
      return true;
   }
   toString() {
      switch (this.scope) {
         case MegaloValueObjectScopes.global:
            return megalo_objects[this.index];
            break;
         case MegaloValueObjectScopes.player:
            return megalo_players[this.which] + ".Object[" + this.index + "]";
            break;
         case MegaloValueObjectScopes.object:
            return megalo_objects[this.which] + ".Object[" + this.index + "]";
            break;
         case MegaloValueObjectScopes.team:
            return megalo_teams[this.which] + ".Object[" + this.index + "]";
            break;
         case MegaloValueObjectScopes.player_slave:
            return megalo_players[this.which] + ".Slave";
         case MegaloValueObjectScopes.player_player_slave:
            return megalo_players[this.which] + ".Player[" + this.index + "].Slave";
            break;
         case MegaloValueObjectScopes.object_player_slave:
            return megalo_objects[this.which] + ".Player[" + this.index + "].Slave";
            break;
         case MegaloValueObjectScopes.team_player_slave:
            return megalo_teams[this.which] + ".Player[" + this.index + "].Slave";
            break;
      }
   }
}

const MegaloValueTeamScopes = {
   global: 0,
   player: 1,
   object: 2,
   team:   3,
   player_owner_team: 4,
   object_owner_team: 5,
};
class MegaloValueTeam extends MegaloValue {
   constructor() {
      super();
      this.scope = null; // e.g. Object.Player
      this.which = null; // e.g. WHICH scope e.g. which object/player/team
      this.index = null; // e.g. variable index in scope; constant value; etc.
   }
   static /*MegaloValue*/ factory(stream) /*noexcept*/ {
      return new MegaloValueTeam();
   }
   /*bool*/ read(stream) {
      this.scope = stream.readBits(_bitcount(Object.keys(MegaloValueTeamScopes).length - 1));
      let variable_scope = null;
      switch (this.scope) {
         case MegaloValueTeamScopes.global:
            this.index = stream.readBits(MegaloVariableScopeTeam.which_bits());
            break;
         case MegaloValueTeamScopes.player:
            variable_scope = MegaloVariableScopePlayer;
            break;
         case MegaloValueTeamScopes.object:
            variable_scope = MegaloVariableScopeObject;
            break;
         case MegaloValueTeamScopes.team:
            variable_scope = MegaloVariableScopeTeam;
            break;
         case MegaloValueTeamScopes.player_owner_team:
            this.which = stream.readBits(MegaloVariableScopePlayer.which_bits());
            break;
         case MegaloValueTeamScopes.object_owner_team:
            this.which = stream.readBits(MegaloVariableScopeObject.which_bits());
            break;
         default:
            console.warn("Team value failed to load: unknown scope " + this.scope + ".");
            return false;
      }
      if (variable_scope) {
         //this.scope = variable_scope;
         this.which = stream.readBits(variable_scope.which_bits());
         this.index = stream.readBits(variable_scope.index_bits("team"));
         return true;
      }
      return true;
   }
   toString() {
      switch (this.scope) {
         case MegaloValueTeamScopes.global:
            return megalo_teams[this.index];
            break;
         case MegaloValueTeamScopes.player:
            return megalo_players[this.which] + ".Team[" + this.index + "]";
         case MegaloValueTeamScopes.object:
            return megalo_objects[this.which] + ".Team[" + this.index + "]";
         case MegaloValueTeamScopes.team:
            return megalo_teams[this.which] + ".Team[" + this.index + "]";
         case MegaloValueTeamScopes.player_owner_team:
            return "Team for " + megalo_players[this.which];
         case MegaloValueTeamScopes.object_owner_team:
            return "Team for " + megalo_objects[this.which];
      }
   }
}

const MegaloValueTimerScopes = {
   global: 0,
   player: 1,
   team:   2,
   object: 3,
   round_time: 4,
   sudden_death_time: 5,
   grace_period_time: 6,
};
class MegaloValueTimer extends MegaloValue {
   constructor() {
      super();
      this.scope = null; // e.g. Object.Player
      this.which = null; // e.g. WHICH scope e.g. which object/player/team
      this.index = null; // e.g. variable index in scope; constant value; etc.
   }
   static /*MegaloValue*/ factory(stream) /*noexcept*/ {
      return new MegaloValueTimer();
   }
   /*bool*/ read(stream) {
      this.scope = stream.readBits(_bitcount(Object.keys(MegaloValueTimerScopes).length - 1));
      let variable_scope = null;
      switch (this.scope) {
         case MegaloValueTimerScopes.global:
            variable_scope = MegaloVariableScopeGlobal;
            break;
         case MegaloValueTimerScopes.player:
            variable_scope = MegaloVariableScopePlayer;
            break;
         case MegaloValueTimerScopes.object:
            variable_scope = MegaloVariableScopeObject;
            break;
         case MegaloValueTimerScopes.team:
            variable_scope = MegaloVariableScopeTeam;
            break;
         case MegaloValueTimerScopes.round_time:
         case MegaloValueTimerScopes.sudden_death_time:
         case MegaloValueTimerScopes.grace_period_time:
            break;
      }
      if (variable_scope) {
         //this.scope = variable_scope;
         this.which = stream.readBits(variable_scope.which_bits());
         this.index = stream.readBits(variable_scope.index_bits("timer"));
         return true;
      }
      return true;
   }
   toString() {
      switch (this.scope) {
         case MegaloValueTimerScopes.global:
            return "Global.Timer[" + this.index + "]";
         case MegaloValueTimerScopes.player:
            return megalo_players[this.which] + ".Timer[" + this.index + "]";
         case MegaloValueTimerScopes.object:
            return megalo_objects[this.which] + ".Timer[" + this.index + "]";
         case MegaloValueTimerScopes.team:
            return megalo_teams[this.which] + ".Timer[" + this.index + "]";
         case MegaloValueTimerScopes.round_time:
            return "Round Time";
         case MegaloValueTimerScopes.sudden_death_time:
            return "Sudden Death Time";
         case MegaloValueTimerScopes.grace_period_time:
            return "Grace Period Time";
      }
   }
}












