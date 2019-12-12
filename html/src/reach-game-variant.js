const REACH_ENGINE = Object.freeze({
   campaign:    0,
   firefight:   1,
   forge:       2,
   multiplayer: 3,
   theater:     4,
});

const ReachContentType = Object.freeze({ // int8_t
   none:        -1,
   dlc:          0,
   save:         1,
   screenshot:   2,
   film:         3,
   film_clip:    4,
   map_variant:  5,
   game_variant: 6,
   unknown_7:    7,
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

class ReachFileChunk {
   constructor(signature, data) {
      this.signature = signature;
      this.size    = 0;
      this.version = 0;
      this.flags   = 0;
      this.data = data;
   }
   parse(stream) {
      let signature = stream.readString(4);
      if (signature != this.signature)
         throw new Error(`Expected chunk signature "${this.signature}"; got "${signature}".`);
      this.size = stream.readUInt32();
      // TODO: validate, bswap the size
      this.version = _byteswap_ushort(stream.readUInt16()); // 08
      this.flags   = _byteswap_ushort(stream.readUInt16()); // 0A
   }
}

class ReachGameVariant {
   constructor() {
      this.blamHeader = new ReachFileChunk('_blf', {
         unk0E: 0,
         unk0E: [],
         unk2E: 0,
      });
      this.contentHeader = new ReachFileChunk('chdr', {
         build: {
            major: 0, // chdr:0C
            minor: 0, // chdr:0E
         },
         contentType: 6, // ReachContentType
         fileLength:  0,
         unk08: BigInt(0),
         unk10: BigInt(0),
         unk18: BigInt(0),
         unk20: BigInt(0),
         activity: 0,
         gameMode: 3,
         engine:   2,
         unk2C:   -1,
         engineCategory: 7,
         createdBy:  new VariantContentAuthor(),
         modifiedBy: new VariantContentAuthor(),
         title:       "Untitled",
         description: "",
         engineIcon: 7,
         unk284: new Array(0x2C), // 0x2C bytes
      });
      this.multiplayer = new ReachFileChunk('mpvr', {
         hashSHA1: new Array(0x14), // 0x14 bytes
         // skip 8 bytes here
         type:     new Bitnumber(4), // same enum as engine
         encodingVersion: new Bitnumber(32, 0, { swap: true }), // 0x6A == vanilla; 0x6B == TU1
         engineVersion:   new Bitnumber(32, 0, { swap: true }),
         header: {
            type:        new Bitnumber(4, 0, { offset: 1 }),
            // data that follows is byte-aligned
            fileLength:  new Bitnumber(32, 0, { swap: true }),
            unk08:       new Bitnumber(64),
            unk10:       new Bitnumber(64),
            unk18:       new Bitnumber(64),
            unk20:       new Bitnumber(64),
            // data stops being byte-aligned
            activity:    new Bitnumber(3, 0, { offset: 1 }),
            gameMode:    new Bitnumber(3, 3),
            engine:      new Bitnumber(3, 2),
            unk2C:       new Bitnumber(32),
            engineCategory: new Bitnumber(8, 0, { signed: true }),
            createdBy:   new VariantContentAuthor(),
            modifiedBy:  new VariantContentAuthor(),
            title:       "",
            description: "",
            unk280:     new Bitnumber(32, 0, { signed: true }), // only for films and film clips
            engineIcon: new Bitnumber(8), // only for game variants
            hopperID:   new Bitnumber(16), // only for activity == 2
         },
         flags: new Bitnumber(1),
         options: {
            misc: {
               flags:           new Bitnumber(4),
               timeLimit:       new Bitnumber(8), // uint8_t
               roundLimit:      new Bitnumber(5),
               roundsToWin:     new Bitnumber(4),
               suddenDeathTime: new Bitnumber(7),
               gracePeriod:     new Bitnumber(5), // must not equal zero
            },
            respawn: {
               flags:             new Bitnumber(4),
               livesPerRound:     new Bitnumber(6),
               teamLivesPerRound: new Bitnumber(7),
               respawnTime:       new Bitnumber(8), // uint8_t
               suicidePenalty:    new Bitnumber(8), // uint8_t
               betrayalPenalty:   new Bitnumber(8), // uint8_t
               respawnGrowth:     new Bitnumber(4),
               loadoutCamTime:    new Bitnumber(4, 10),
               traitsDuration:    new Bitnumber(6,  5),
               traits:            new GameVariantPlayerTraits(),
            },
            social: {
               observers:   new Bitbool(false),
               teamChanges: new Bitnumber(2),
               flags:       new Bitnumber(5, 0, { flags: SocialFlags }),
            },
            map: {
               flags:      new Bitnumber(6, 0, { flags: MapOverrideFlags }),
               baseTraits: new GameVariantPlayerTraits(),
               weaponSet:  new Bitnumber(8, 0, { signed: true }), // int8_t,
               vehicleSet: new Bitnumber(8, 0, { signed: true }),
               powerups: {
                  //
                  // Read all traits first, then all durations.
                  //
                  red: {
                     duration: new Bitnumber(7,  6),
                     traits:   new GameVariantPlayerTraits(),
                  },
                  blue: {
                     duration: new Bitnumber(7, 30),
                     traits:   new GameVariantPlayerTraits(),
                  },
                  yellow: {
                     duration: new Bitnumber(7, 10),
                     traits:   new GameVariantPlayerTraits(),
                  },
               },
            },
            team: {
               scoringMethod: new Bitnumber(3),
               modelOverride: new Bitnumber(3, 0, { enum: PlayerSpecies }),
               designatorSwitchType: new Bitnumber(2),
               teams: // std::array<GameVariantTeam, 8>
                  (function() {
                     let t = [];
                     for(let i = 0; i < 8; i++)
                        t[i] = new GameVariantTeam();
                     return t;
                  }),
            },
            loadouts: {
               flags:    new Bitnumber(2, 0, { flags: LoadoutOptionFlags }),
               palettes: // std::array<GameVariantLoadoutPalette, 6>
                  (function() {
                     let t = [];
                     for(let i = 0; i < 6; i++)
                        t[i] = new GameVariantLoadoutPalette();
                     return t;
                  }),
            },
         },
         scriptData: {
            traits:  [], // std::vector<GameVariantMegaloPlayerTraits>, max 16
            options: [], // std::vector<GameVariantMegaloOption>, max 16
            strings: new LocalizedStringTable(112, 0x4C00),
         },
         stringTableIndexPointer: 0, // TODO: make and use a string table index pointer type
         nameStringTable:     new LocalizedStringTable(1, 0x180),
         descStringTable:     new LocalizedStringTable(1, 0xC00),
         categoryStringTable: new LocalizedStringTable(1, 0x180),
         engineIcon:     new Bitnumber(32 - 1, 0, { offset: 1 }),
         engineCategory: new Bitnumber(32 - 1, 0, { offset: 1 }),
         mapPermissions: new GameVariantMapPermissions(),
         playerRatingParams: new GameVariantPlayerRatingParams(),
         scoreToWin: new Bitnumber(16, 0),
         unkF7A6:    new Bitbool(),
         unkF7A7:    new Bitbool(),
         optionToggles: {
            engine: {
               disabled: new GameVariantEngineOptionToggleList(),
               hidden:   new GameVariantEngineOptionToggleList(),
            },
            megalo: {
               disabled: new GameVariantMegaloOptionToggleList(),
               hidden:   new GameVariantMegaloOptionToggleList(),
            },
         },
         //
         // TODO: megalo here?
         //
         titleUpdateData: new GameVariantTU1Data(), // only if encodingVersion >= 0x6B
      });
   }
   parse(stream) {
      {  // _blf
         let c = this.blamHeader;
         let d = c.data;
         c.parse(stream);
         d.unk0C = stream.readUInt16();
         d.unk0E = stream.readBytes(0x20);
         d.unk2E = stream.readBytes(0x02);
      }
      {  // chdr
         let c = this.contentHeader;
         let d = c.data;
         c.parse(stream);
         d.build.major = stream.readUInt16(); // 0C
         d.build.minor = stream.readUInt16(); // 0E
         d.contentType = stream.readSInt8(); // gvar_content_type
         stream.skipBytes(3);
         d.fileLength = stream.readUInt32();
         d.unk08    = stream.readUInt64();
         d.unk10    = stream.readUInt64();
         d.unk18    = stream.readUInt64();
         d.unk20    = stream.readUInt64();
         d.activity = stream.readSInt8(); // gvar_activity
         d.gameMode = stream.readUInt8(); // gvar_game_mode
         d.engine   = stream.readUInt8(); // gvar_game_engine
         stream.skipBytes(1);
         d.unk2C = stream.readUInt32();
         d.engineCategoryIndex = stream.readUInt32();
         d.pad34 = stream.readUInt32();
         d.createdBy.parseBytes(stream);
         d.modifiedBy.parseBytes(stream);
         stream.assertBytePos(0x0C0);
         d.title       = stream.readWidecharString(128);
         stream.assertBytePos(0x1C0);
         d.description = stream.readWidecharString(128);
         stream.assertBytePos(0x2C0);
         d.engineIconIndex = stream.readUInt32(); // 280
         d.unk284 = stream.readBytes(0x2C);
         stream.assertBytePos(0x2F0);
      }
      {  // mpvr
         let c = this.multiplayer;
         let d = c.data;
         c.parse(stream);
         d.hashSHA1 = stream.readBytes(0x14);
         stream.skipBytes(4);
         stream.skipBytes(4);
         d.type.read(stream);
         d.encodingVersion.read(stream);
         d.engineVersion.read(stream);
         if (d.encodingVersion < 0x6A) // base game
            console.warn(`Game variant encoding version ${d.encodingVersion.toString(16)} is suspiciously low...`);
         if (d.encodingVersion > 0x6B) // TU1
            console.warn(`Game variant encoding version ${d.encodingVersion.toString(16)} is suspiciously high...`);
         {
            let h = d.header;
            h.type.read(stream);
            h.fileLength.read(stream);
            h.unk08.read(stream);
            h.unk10.read(stream);
            h.unk18.read(stream);
            h.unk20.read(stream);
            h.activity.read(stream);
            h.gameMode.read(stream);
            h.engine.read(stream);
            h.unk2C.read(stream);
            h.engineCategory.read(stream);
            h.createdBy.parseBits(stream);
            h.modifiedBy.parseBits(stream);
            //
            let endianness = stream.endianness;
            stream.endianness = ENDIAN_BIG; // KSoft.Tool always uses big-endian, but MCC may be little-endian for this?
            h.title       = stream.readWidecharString(128, true);
            h.description = stream.readWidecharString(128, true);
            stream.endianness = endianness;
            //
            switch (h.type) {
               case gvar_content_type.film:
               case gvar_content_type.film_clip:
                  h.unk280.read(stream);
                  break;
               case gvar_content_type.game_variant:
                  h.engineIcon.read(stream);
                  break;
            }
            if (h.activity == 2)
               h.hopperID.read(stream);
            if (h.gameMode == gvar_game_mode.campaign) {
               throw new Error("Campaign not implemented!");
            } else if (h.gameMode == gvar_game_mode.survival) {
               throw new Error("Firefight not implemented!");
            }
         }
         d.flags.read(stream);
         {
            let o = d.options;
            for(let j in o.misc)
               o.misc[j].read(stream);
            for(let j in o.respawn) {
               let option = o.respawn[j];
               let method = option.read || option.parse; // TODO: make consistent
               method.call(option, stream);
            }
            for(let j in o.social)
               o.social[j].read(stream);
            {
               let m = o.map;
               m.flags.read(stream);
               m.baseTraits.parse(stream);
               m.weaponSet.read(stream);
               m.vehicleSet.read(stream);
               m.powerups.red.traits.parse(stream);
               m.powerups.blue.traits.parse(stream);
               m.powerups.yellow.traits.parse(stream);
               m.powerups.red.duration.read(stream);
               m.powerups.blue.duration.read(stream);
               m.powerups.yellow.duration.read(stream);
            }
            {
               let t = o.team;
               t.scoringMethod.read(stream);
               t.modelOverride.read(stream);
               t.designatorSwitchType.read(stream);
               for(let i = 0; i < t.teams.length; i++)
                  t.teams[i].parse(stream);
            }
            o.loadouts.flags.read(stream);
            for(let i = 0; i < o.loadouts.palettes.length; i++)
               o.loadouts.palettes[i].parse(stream);
         }
         {  // player traits (megalo-specific)
            //
            // Alpha Zombies should have four: infected, alpha, last man, and haven
            //
            let list = d.scriptData.traits;
            let count = stream.readBits(_bitcount(16), false);
            for(let i = 0; i < count; i++)
               list[i] = new GameVariantMegaloPlayerTraits(stream);
         }
         {  // user-defined options
            let list = d.scriptData.options
            let count = stream.readBits(_bitcount(16), false); // 16 options max
            for(let i = 0; i < count; i++)
               list[i] = new GameVariantRawMegaloOption(stream); // TODO: not raw
         }
         d.scriptData.strings.parse(stream);
         d.stringTableIndexPointer = stream.readMegaloVariantStringIndex() - 1;
         d.engineIcon.read(stream);
         d.engineCategory.read(stream);
         d.mapPermissions.parse(stream);
         d.playerRatingParams.parse(stream);
         d.scoreToWin.read(stream);
         d.unkF7A6.read(stream);
         d.unkF7A7.read(stream);
         for(let i in d.optionToggles) {
            let type = d.optionToggles[i];
            for(let j in type)
               type[j].parse(stream);
         }
         //
         // TODO: megalo here?
         //
         if (this.encodingVersion >= 0x6B) // TU1 encoding version (stock is 0x6A)
            this.titleUpdateData.parse(stream);
      }
   }
}