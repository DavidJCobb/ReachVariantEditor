const FIREFIGHT_SQUAD_TYPES = { // these correspond to the Waves list in the SMDT (Survival Mode Globals) tag
   BRUTES:               0, // Brutes only
   BRUTE_KILL_TEAM:      1, // Brutes led by a chieftain
   BRUTE_PATROL:         2, // Brutes and Grunts
   BRUTE_INFANTRY:       3, // Brutes and Jackals
   BRUTE_TACTICAL:       4, // Brutes and Skirmishers
   BRUTE_CHIEFTAINS:     5, // Brute chieftains only
   ELITES:               6, // Elites only
   ELITE_PATROL:         7, // Elites and Grunts
   ELITE_INFANTRY:       8, // Elites and Jackals
   ELITE_AIRBORNE:       9, // Elites only, with jetpacks
   ELITE_TACTICAL:      10, // Elites and Skirmishers
   ELITE_SPEC_OPS:      11, // Elite spec-ops only
   ENGINEERS:           12, // a single Engineer (not in UI)
   ELITE_GENERALS:      13, // Elite generals only
   GRUNT:               14,
   HUNTER_KILL_TEAM:    15, // Hunters and Elites and Grunts
   HUNTER_PATROL:       16, // Hunters and Grunts
   HUNTER_STRIKE_TEAM:  17, // Hunters and Elites led by a general
   JACKAL_PATROL:       18, // Jackals and Grunts
   ELITE_STRIKE_TEAM:   19, // Elites led by a general
   SKIRMISHER_PATROL:   20, // Skirmishers and Grunts
   HUNTERS:             21, // Hunters only
   JACKAL_SNIPERS:      22, // Jackals with Needle Rifles and Focus Rifles
   JACKALS:             23, // Jackals only
   HUNTER_INFANTRY:     24, // Hunters and Jackals
   GUTA:                25, // a single Guta ("Mule" internally) (only on supported maps, and none seem to) (not in UI)
   SKIRMISHERS:         26, // Skirmishers only
   HUNTER_TACTICAL:     27, // Hunters and Skirmishers
   SKIRMISHER_INFANTRY: 28, // Skirmishers and Jackals
   HERETICS:            29, // Elites wielding human weapons, varying with difficulty (assault rifle, DMR, grenade launcher, magnum, shotgun)
   HERETIC_SNIPERS:     30, // Elites wielding human sniper rifles
   HERETIC_HEAVY:       31, // Elites wielding human explosive weaponry
};

const FF_WAVE_DAMAGE_MODIFIER = [
    -1, // unchanged
     0, // 0%
    25, // 25%
    50, // 50%
    75, // 75%
    90, // 90%
   100,
   110,
   125,
   150,
   200,
   300
];
const FF_WAVE_DAMAGE_RESISTANCE = [
     -1, // unchanged
     10, // 10%
     50, // 50%
     90,
    100,
    110,
    150,
    200,
    300,
    500,
   1000,
   2000,
   Infinity // invulnerable
];
const FF_WAVE_GRENADES = [
   "Unchanged",
   "Normal",
   "None",
   "Catch"
];
const FF_WAVE_HEARING = [
   "Unchanged",
   "Normal",
   "Deaf",
   "Sharp"
];
const FF_WAVE_LUCK = [
   "Unchanged",
   "Normal",
   "Unlucky",
   "Lucky",
   "Leprechaun"
];
const FF_WAVE_SHOOTINESS = [
   "Unchanged",
   "Normal",
   "Marksman",
   "Trigger Happy"
];
const FF_WAVE_VISION = [
   "Unchanged",
   "Normal",
   "Blind", // not normally visible in the UI
   "Nearsighted",
   "Eagle Eye"
];
const FF_WAVE_TRAIT_BOOL = [
   "Unchanged",
   "Disabled",
   "Enabled"
];

class CustomGameRespawnOptions {
   constructor(stream) {
      this.flags             = stream.readBits(4, false);
      this.livesPerRound     = stream.readBits(6, BIT_ORDER_UNKNOWN);
      this.teamLivesPerRound = stream.readBits(7, BIT_ORDER_UNKNOWN);
      this.respawnTime       = stream.readByte();
      this.suicidePenalty    = stream.readByte();
      this.betrayalPenalty   = stream.readByte();
      this.respawnGrowth     = stream.readBits(4, BIT_ORDER_UNKNOWN);
      this.loadoutCamTime    = stream.readBits(4, false); // default 10
      this.traitsDuration    = stream.readBits(6, false);
      this.traits = new GameVariantPlayerTraits(stream);
   }
}

class CustomGameOptions {
   constructor(stream) {
      {  // misc options
         let o = this.misc = {};
         o.flags           = stream.readBits(4, false);
         o.timeLimit       = stream.readByte(); // minutes
         o.roundLimit      = stream.readBits(5, false);
         o.roundsToWin     = stream.readBits(4, BIT_ORDER_UNKNOWN);
         o.suddenDeathTime = stream.readBits(7, BIT_ORDER_UNKNOWN); // seconds
         o.gracePeriod     = stream.readBits(5, BIT_ORDER_UNKNOWN); // value must not be zero
      }
      this.respawn = new CustomGameRespawnOptions(stream);
      {  // social options
         let o = this.social = {};
         o.observers   = stream.readBits(1) != 0;
         o.teamChanges = stream.readBits(2, BIT_ORDER_UNKNOWN);
         o.flags       = SocialFlags.make(stream.readBits(5, false));
      }
      {  // map options
         let o = this.map = {};
         o.flags       = MapOverrideFlags.make(stream.readBits(6, false));
         o.baseTraits  = new GameVariantPlayerTraits(stream);
         o.weaponSet   = stream.readSInt8();
         o.vehicleSet  = stream.readSInt8();
         o.powerups = {
            red: {}, // overshield
            blue: {}, // camo
            yellow: {},
         };
         o.powerups.red.traits    = new GameVariantPlayerTraits(stream);
         o.powerups.blue.traits   = new GameVariantPlayerTraits(stream);
         o.powerups.yellow.traits = new GameVariantPlayerTraits(stream);
         o.powerups.red.duration    = stream.readBits(7, BIT_ORDER_UNKNOWN);
         o.powerups.blue.duration   = stream.readBits(7, BIT_ORDER_UNKNOWN);
         o.powerups.yellow.duration = stream.readBits(7, BIT_ORDER_UNKNOWN);
      }
      {  // team options
         let o = this.team = {};
         o.scoringMethod = stream.readBits(3, BIT_ORDER_UNKNOWN);
         o.modelOverride = PlayerSpecies.make(stream.readBits(3, BIT_ORDER_UNKNOWN));
         o.designatorSwitchType = stream.readBits(2, BIT_ORDER_UNKNOWN);
         o.teams = [];
         for(let i = 0; i < 8; i++)
            o.teams[i] = new GameVariantTeam(stream);
      }
      {  // loadout options
         let o = this.loadouts = {};
         o.flags    = LoadoutOptionFlags.make(stream.readBits(2, BIT_ORDER_UNKNOWN));
         o.palettes = [];
         for(let i = 0; i < 6; i++)
            o.palettes[i] = new GameVariantLoadoutPalette(stream);
      }
   }
}

class FirefightWaveTraits {
   constructor(stream) {
      this.vision     = stream.readBits(3, false); // max value is 4; if greater, treated as 0
      this.hearing    = stream.readBits(2, false); // max value is 3; if greater, treated as 0
      this.luck       = stream.readBits(3, false); // max value is 4; if greater, treated as 0
      this.shootiness = stream.readBits(2, false); // max value is 3; if greater, treated as 0
      this.grenades   = stream.readBits(2, false); // max value is 3; if greater, treated as 0
      this.dontDropEquipment = stream.readBits(2, false); // max value is 2; if greater, treated as 0 // it's the opposite of how the UI displays it
      this.assassinImmunity  = stream.readBits(2, false); // max value is 2; if greater, treated as 0 // unchanged/enabled/disabled
      this.headshotImmunity  = stream.readBits(2, false); // max value is 2; if greater, treated as 0 // unchanged/enabled/disabled
      this.damageResistance  = stream.readBits(4, false); // enum
      this.damageModifier    = stream.readBits(4, false); // enum
   }
}
class FirefightVariantData {
   constructor(stream) {
      this.header    = new UGCHeader(stream);
      this.isBuiltIn = stream.readBits(1);
      this.options   = new CustomGameOptions(stream); // Friendly Fire and Betrayal Booting are in here
      this.unkW = stream.readBits(5, false); // scenario flags?!
      this.unkX = stream.readBits(3, false);
      this.unkA = stream.readBits(8, false);
      this.waveLimit = stream.readBits(4, false);
      this.unkB = stream.readBits(15, false); // a quantity of points
      this.turnCount = stream.readBits(15, false); // verify this, because 15 bits is absurd when the UI only goes up to the value "3"
      this.unkD = stream.readBits(7, false) - 1;
      this.unkE = stream.readBits(7, false) - 1;
      this.unkF = stream.readBits(15, false);
      this.unkG = stream.readBits(7, false) - 1; // Spartan max lives?
      this.generatorCount = stream.readBits(2, false);
      this.baseTraitsSpartan = new GameVariantPlayerTraits(stream); // assumed
      this.baseTraitsElite   = new GameVariantPlayerTraits(stream); // assumed
      this.waveTraits = new FirefightWaveTraits(stream);
      this.skulls = []; // red, yellow, blue
      for(let i = 0; i < 3; ++i) {
         let current = this.skulls[i] = {};
         current.traitsSpartan = new GameVariantPlayerTraits(stream); // assumed
         current.traitsElite   = new GameVariantPlayerTraits(stream); // assumed
         current.waveTraits = new FirefightWaveTraits(stream);
      }
      this.respawnSettings = new CustomGameRespawnOptions(stream); // most likely for Elites
      this.rounds = [];
      for(let i = 0; i < 3; ++i) {
         let current = this.rounds[i] = {
            skulls: 0,
            waveInitial: {},
            waveMain:    {},
            waveBoss:    {}
         };
         //
         function loadWave(current, stream) {
            current.usesDropship = stream.readBits(1);
            current.squadOrder   = stream.readBits(1); // 0 = random, 1 = ordered
            current.squadCount   = stream.readBits(4, false); // not shown in UI
            current.squads = []; // assumed
            for(let j = 0; j < 12; ++j)
               current.squads[j] = stream.readBits(8, false) - 1;
         }
         //
         current.skulls = stream.readBits(18, false); // assumed
         loadWave(current.waveInitial, stream);
         loadWave(current.waveMain, stream);
         loadWave(current.waveBoss, stream);
      }
      this.bonusWave = {}; // assumed
      this.bonusWave.duration = stream.readBits(12, false);
      this.bonusWave.skulls   = stream.readBits(18, false); // assumed
      this.bonusWave.usesDropship = stream.readBits(1);
      this.bonusWave.squadOrder   = stream.readBits(1); // 0 = random, 1 = ordered
      this.bonusWave.squadCount   = stream.readBits(4, false);
      this.bonusWave.squads = []; // assumed
      for(let j = 0; j < 12; ++j)
         this.bonusWave.squads[j] = stream.readBits(8, false) - 1;
   }
}
class MegaloVariantData {
   constructor(stream) {
      this.encodingVersion = _byteswap_ulong(stream.readUInt32());
      this.engineVersion   = _byteswap_ulong(stream.readUInt32()); // build revision version ID
      if (this.encodingVersion < 0x6A) // base game
         console.warn(`Game variant encoding version ${this.encodingVersion.toString(16)} is suspiciously low...`);
      if (this.encodingVersion > 0x6B) // TU1
         console.warn(`Game variant encoding version ${this.encodingVersion.toString(16)} is suspiciously high...`);
      this.header    = new UGCHeader(stream);
      this.isBuiltIn = stream.readBits(1);
      this.options   = new CustomGameOptions(stream);
      {  // player traits (megalo-specific)
         //
         // Alpha Zombies should have four: infected, alpha, last man, and haven
         //
         let o = this.traits = [];
         let count = stream.readBits(_bitcount(16), false);
         for(let i = 0; i < count; i++)
            o[i] = new GameVariantMegaloPlayerTraits(stream);
      }
      {  // user-defined options
         let o = this.megaloOptions = [];
         let count = stream.readBits(_bitcount(16), false); // 16 options max
         for(let i = 0; i < count; i++)
            this.megaloOptions[i] = new GameVariantRawMegaloOption(stream);
      }
      {  // string table
         this.stringTable = new LocalizedStringTable(112, 0x4C00);
         this.stringTable.parse(stream);
      }
      this.stringTableIndexPointer = stream.readMegaloVariantStringIndex() - 1;
      {  // description locale strings
         {  // name string
            this.nameStringTable = new LocalizedStringTable(1, 0x180);
            this.nameStringTable.parse(stream);
         }
         {  // description string
            this.descStringTable = new LocalizedStringTable(1, 0xC00);
            this.descStringTable.parse(stream);
         }
         {  // category string
            this.categoryStringTable = new LocalizedStringTable(1, 0x180);
            this.categoryStringTable.parse(stream);
         }
      }
      this.engineIconIndex = stream.readBits(_bitcount(32 - 1), BIT_ORDER_UNKNOWN) - 1;
      this.engineCategory  = stream.readBits(_bitcount(32 - 1), BIT_ORDER_UNKNOWN) - 1;
      this.mapPermissions = new GameVariantMapPermissions(stream);
      this.playerRatingParams = new GameVariantPlayerRatingParams(stream);
      this.scoreToWin = _byteswap_ushort(stream.readUInt16());
      this.unkF7A6 = stream.readBits(1) != 0;
      this.unkF7A7 = stream.readBits(1) != 0;
      console.log("Stream bit pos: " + stream.offset);
      {  // option toggles
         this.optionToggles = {
            engine: {
               disabled: new GameVariantEngineOptionToggleList(stream),
               hidden:   new GameVariantEngineOptionToggleList(stream),
            },
            megalo: {
               disabled: new GameVariantMegaloOptionToggleList(stream),
               hidden:   new GameVariantMegaloOptionToggleList(stream),
            },
         };
      }
      //
      // KSoft.Tool code suggests that megalo data goes here, but if 
      // that's true, then how come we seem to be getting perfect TU 
      // data as-is? A non-TU variant had 0.76 bloom, a TU variant 
      // had 0.65, and that makes the TU variant 85% of the other.
      //
      {  // megalo
         this.conditions = [];
         let count = stream.readBits(_bitcount(512));
         for(let i = 0; i < count; i++) {
            let c = new MegaloCondition();
            this.conditions.push(c);
            if (!c.read(stream)) {
               console.warn("Failed decode of condition " + i + ".");
               break;
            }
         }
      
         // conditions vector
            // condition
               // serializeImpl
               // arguments
         // actions vector
         // triggers vector
         // game statistics vector
         // global vars
         // player vars
         // object vars
         // team vars
         // HUD widgets vector
         // trigger entry points
         // object type references (as bitset)
         // object filters vector
         // game-object filters
            // game object filter count
            // candy spawner filters vector
         
         // BeginDecompile();
         // EndDecompile();
      }
      if (this.encodingVersion >= 0x6B) // TU1 encoding version (stock is 0x6A)
         this.titleUpdateData = new GameVariantTU1Data(stream);
      else
         this.titleUpdateData = null;
   }
}

class GameVariantMultiplayerData {
   constructor(stream) {
      stream.endianness = ENDIAN_LITTLE;
      this.signature = stream.readString(4);
      if (this.signature != "mpvr")
         throw new Error(`GameVariantMultiplayerData expected signature "mpvr"; got "${this.signature}".`);
      let size = stream.readUInt32();
      if (size != 0x5028) {
         size = _byteswap_ulong(size);
         if (size != 0x5028)
            throw new Error(`GameVariantMultiplayerData expected size 0x5028; got 0x${_byteswap_ulong(size).toString(16)} or 0x${size}.toString(16).`);
      }
      this.chunkVersion = _byteswap_ushort(stream.readUInt16()); // 08
      this.chunkFlags   = _byteswap_ushort(stream.readUInt16()); // 0A
      //
      this.hashSHA1 = stream.readBytes(0x14);
      stream.skipBytes(4);
      stream.skipBytes(4);
      this.type = stream.readBits(4, BIT_ORDER_UNKNOWN); // gvar_game_engine // this may go before one or both of the previous skipped dwords
      //
      if (this.type == 1) {
         this.data = new MegaloVariantData(stream);
         // TODO: Forge data
      } else if (this.type == 2) {
         this.data = new MegaloVariantData(stream);
      } else if (this.type == 4) {
         this.data = new FirefightVariantData(stream);
      }
      //
   }
}