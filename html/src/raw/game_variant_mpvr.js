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
      this.encodingVersion = _byteswap_ulong(stream.readUInt32());
      this.engineVersion   = _byteswap_ulong(stream.readUInt32()); // build revision version ID
      if (this.encodingVersion < 0x6A) // base game
         console.warn(`Game variant encoding version ${this.encodingVersion.toString(16)} is suspiciously low...`);
      if (this.encodingVersion > 0x6B) // TU1
         console.warn(`Game variant encoding version ${this.encodingVersion.toString(16)} is suspiciously high...`);
      {  // header - same data as CHDR but fields aren't byte-aligned here
         let o = this.header = {};
         o.type       = stream.readBits(4, BIT_ORDER_UNKNOWN) - 1; // gvar_content_type
            // ^ "None" is encoded as 0 instead of -1 in mpvr; the whole enum is shifted up by 1
            //   when written to the file.
         // Note: As of the previous field, we are byte-aligned again.
         o.fileLength = _byteswap_ulong(stream.readUInt32());
         stream.reportOffset();
         o.unk08      = _byteswap_uint64(stream.readUInt64());
         o.unk10      = _byteswap_uint64(stream.readUInt64());
         o.unk18      = _byteswap_uint64(stream.readUInt64());
         o.unk20      = _byteswap_uint64(stream.readUInt64());
         stream.reportOffset();
         o.activity   = stream.readBits(3, false) - 1;
            // ^ "None" is encoded as 0 instead of -1 in mpvr; the whole enum is shifted up by 1
            //   when written to the file.
         o.gameMode   = stream.readBits(3, false);
         o.engine     = stream.readBits(3, false);
         o.unk2C      = stream.readUInt32();
         o.engineCategoryIndex = stream.readBits(8, false); // TODO: we need to sign-extend it from one byte to an SInt32
         o.createdBy   = new VariantContentAuthor();
         o.createdBy.parseBits(stream);
         o.modifiedBy  = new VariantContentAuthor();
         o.modifiedBy.parseBits(stream);
         let endianness = stream.endianness;
         stream.endianness = ENDIAN_BIG; // KSoft.Tool always uses big-endian, but MCC may be little-endian for this?
         o.title       = stream.readWidecharString(128, true);
         o.description = stream.readWidecharString(128, true);
         stream.endianness = endianness;
         //
         switch (o.type) {
            case gvar_content_type.film:
            case gvar_content_type.film_clip:
               o.unk280 = stream.readSInt32();
               break;
            case gvar_content_type.game_variant:
               o.unk280 = stream.readBits(8, BIT_ORDER_UNKNOWN); // engineIconIndex
               break;
         }
         if (o.activity == 2)
            o.hopperID = stream.readUInt16(); // TODO: TEST ME (how?)
         else
            o.hopperID = null;
         if (o.gameMode == gvar_game_mode.campaign) {
            throw new Error("Campaign not implemented!");
         } else if (o.gameMode == gvar_game_mode.survival) {
            throw new Error("Firefight not implemented!");
         }
      }
      this.flags = stream.readBits(1);
      this.options = {};
      {  // misc options
         let o = this.options.misc = {};
         o.flags           = stream.readBits(4, false);
         o.timeLimit       = stream.readByte(); // minutes
         o.roundLimit      = stream.readBits(5, false);
         o.roundsToWin     = stream.readBits(4, BIT_ORDER_UNKNOWN);
         o.suddenDeathTime = stream.readBits(7, BIT_ORDER_UNKNOWN); // seconds
         o.gracePeriod     = stream.readBits(5, BIT_ORDER_UNKNOWN); // value must not be zero
      }
      {  // respawn options
         let o = this.options.respawn = {};
         o.flags             = stream.readBits(4, false);
         o.livesPerRound     = stream.readBits(6, BIT_ORDER_UNKNOWN);
         o.teamLivesPerRound = stream.readBits(7, BIT_ORDER_UNKNOWN);
         o.respawnTime       = stream.readByte();
         o.suicidePenalty    = stream.readByte();
         o.betrayalPenalty   = stream.readByte();
         o.respawnGrowth     = stream.readBits(4, BIT_ORDER_UNKNOWN);
         o.loadoutCamTime    = stream.readBits(4, false); // default 10
         o.traitsDuration    = stream.readBits(6, false);
         o.traits = new GameVariantPlayerTraits(stream);
      }
      {  // social options
         let o = this.options.social = {};
         o.observers   = stream.readBits(1) != 0;
         o.teamChanges = stream.readBits(2, BIT_ORDER_UNKNOWN);
         o.flags       = SocialFlags.make(stream.readBits(5, false));
      }
      {  // map options
         let o = this.options.map = {};
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
         let o = this.options.team = {};
         o.scoringMethod = stream.readBits(3, BIT_ORDER_UNKNOWN);
         o.modelOverride = PlayerSpecies.make(stream.readBits(3, BIT_ORDER_UNKNOWN));
         o.designatorSwitchType = stream.readBits(2, BIT_ORDER_UNKNOWN);
         o.teams = [];
         for(let i = 0; i < 8; i++)
            o.teams[i] = new GameVariantTeam(stream);
      }
      {  // loadout options
         let o = this.options.loadouts = {};
         o.flags    = LoadoutOptionFlags.make(stream.readBits(2, BIT_ORDER_UNKNOWN));
         o.palettes = [];
         for(let i = 0; i < 6; i++)
            o.palettes[i] = new GameVariantLoadoutPalette(stream);
      }
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