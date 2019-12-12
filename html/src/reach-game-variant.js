const REACH_ENGINE = Object.freeze({
   campaign:    0,
   firefight:   1,
   forge:       2,
   multiplayer: 3,
   theater:     4,
});

let SocialFlags = new TypedBitfield({
   name: "SocialFlags",
}, {
   friendlyFire:     0x01,
   betrayalBooting:  0x02,
   proximityVoice:   0x04,
   openChannelVoice: 0x08,
   deadPlayerVoice:  0x10,
});
let MapOverrideFlags = new TypedBitfield({
   name: "MapOverrideFlags",
}, {
   grenades:  0x01, // 1 = enabled, displayed as "Map Default"
   shortcuts: 0x02, // for Race, I assume
   equipment: 0x04, // 1 = enabled, displayed as "Map Default"
   powerups:  0x08, // 1 = enabled, displayed as "Map Default"
   turrets:   0x10, // 1 = enabled, displayed as "Map Default"
   indestructibleVehicles: 0x20,
});

let PlayerSpecies = new TypedEnum({
   name: "PlayerSpecies",
}, {
   user_preferred: 0,
   spartan:  1,
   elite:    2,
   team_default: 3,
   survival: 4, // anyone on NONE team is an Elite
});

class ReachGameVariant {
   constructor() {
      this.build = {
         major: 0, // chdr:0C
         minor: 0, // chdr:0E
      };
      this.engine = REACH_ENGINE.multiplayer;
      this.unk08 = BigInt(0);
      this.unk10 = BigInt(0);
      this.unk18 = BigInt(0);
      this.unk20 = BigInt(0);
      this.engineIcon     = 7; // file icon
      this.engineCategory = 7; // file category, for sorting by base gametype in the UI
      this.createdBy  = new VariantContentAuthor();
      this.modifiedBy = new VariantContentAuthor();
      this.lobbyTitle       = ""; // chdr
      this.lobbyDescription = ""; // chdr
      //
      this.encodingVersion = 0x6B; // TU1
      this.engineVersion   = -1; // MCC?
      this.hopperID = null;
      this.titleUpdateData = new GameVariantTU1Data();
      //
      this.unknownOptionFlag = false; // the bit just before the variant options
      this.options = {};
      {  // misc options
         let o = this.options.misc = {};
         o.flags           = 0;
         o.timeLimit       = 0; // minutes
         o.roundLimit      = 0;
         o.roundsToWin     = 0;
         o.suddenDeathTime = 0; // seconds
         o.gracePeriod     = 4; // value must not be zero
      }
      {  // respawn options
         let o = this.options.respawn = {};
         o.flags             = 0;
         o.livesPerRound     = 0;
         o.teamLivesPerRound = 0;
         o.respawnTime       = 5;
         o.suicidePenalty    = 5;
         o.betrayalPenalty   = 5;
         o.respawnGrowth     = 0;
         o.loadoutCamTime    = 10;
         o.traitsDuration    = 5;
         o.traits = new GameVariantRawPlayerTraits(stream); // TODO: not raw
      }
      {  // social options
         let o = this.options.social = {};
         o.observers   = false;
         o.teamChanges = 0;
         o.flags       = SocialFlags.make(0);
      }
      {  // map options
         let o = this.options.map = {};
         o.flags       = MapOverrideFlags.make(0);
         o.baseTraits  = new GameVariantRawPlayerTraits(stream); // TODO: not raw
         o.weaponSet   = 0;
         o.vehicleSet  = 0;
         o.powerups = {
            red: {}, // overshield
            blue: {}, // camo
            yellow: {}, // custom
         };
         o.powerups.red.traits = new GameVariantRawPlayerTraits(stream); // TODO: not raw
         o.powerups.blue.traits = new GameVariantRawPlayerTraits(stream); // TODO: not raw
         o.powerups.yellow.traits = new GameVariantRawPlayerTraits(stream); // TODO: not raw
         o.powerups.red.duration = 6;
         o.powerups.blue.duration = 30;
         o.powerups.yellow.duration = 10;
      }
      {  // team options
         let o = this.options.team = {};
         o.scoringMethod = 0;
         o.modelOverride = PlayerSpecies.make(0);
         o.designatorSwitchType = 0;
         o.teams = [];
         for(let i = 0; i < 8; i++)
            o.teams[i] = new GameVariantTeam();
      }
      {  // loadout options
         let o = this.options.loadouts = {};
         o.flags    = LoadoutOptionFlags.make(3);
         o.palettes = [];
         for(let i = 0; i < 6; i++)
            o.palettes[i] = new GameVariantLoadoutPalette(stream); // TODO: not raw
      }
      {  // player traits (megalo-specific)
         let o = this.traits = []; // max 16
      }
      {  // user-defined options
         let o = this.megaloOptions = []; // max 16
      }
      this.stringTable = new LocalizedStringTable(112, 0x4C00);
      this.stringTableIndexPointer = -1; // TODO: what is this, actually?
      {  // description locale strings
         this.nameStringTable = new LocalizedStringTable(1, 0x180);
         this.descStringTable = new LocalizedStringTable(1, 0xC00);
         this.categoryStringTable = new LocalizedStringTable(1, 0x180);
      }
      this.mapPermissions = new GameVariantMapPermissions();
      this.playerRatingParams = new GameVariantPlayerRatingParams(stream); // TODO: not raw
      this.scoreToWin = 0; // uint16
      this.unkF7A6 = false;
      this.unkF7A7 = false;
      this.optionToggles = {
         engine: {
            disabled: new GameVariantEngineOptionToggleList(),
            hidden:   new GameVariantEngineOptionToggleList(),
         },
         megalo: {
            disabled: new GameVariantMegaloOptionToggleList(),
            hidden:   new GameVariantMegaloOptionToggleList(),
         },
      };
   }
}