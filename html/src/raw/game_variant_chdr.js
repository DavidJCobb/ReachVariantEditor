const gvar_content_type = Object.freeze({ // int8_t
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
const gvar_activity = Object.freeze({ // int8_t
   none:        -1,
   activities:   0,
   campaign:     1,
   survival:     2,
   matchmaking:  3,
   forge:        4,
   theater:      5,
});
const gvar_game_mode = Object.freeze({ // uint8_t
   none:        0,
   campaign:    1,
   survival:    2, // firefight
   multiplayer: 3,
   forge:       4,
   theater:     5,
});
const gvar_game_engine = Object.freeze({ // uint8_t
   none:      0,
   forge:     1,
   megalo:    2,
   campaign:  3,
   survival:  4,
   firefight: 5,
});

class GameVariantContentHeader {
   constructor(stream) {
      stream.endianness = ENDIAN_LITTLE;
      this.signature = stream.readString(4);
      if (this.signature != "chdr")
         throw new Error(`GameVariantContentHeader expected signature "chdr"; got "${this.signature}".`);
      let size = stream.readUInt32();
      if (size != 0x2C0) {
         size = _byteswap_ulong(size);
         if (size != 0x2C0)
            throw new Error(`GameVariantContentHeader expected size 0x2C0; got 0x${_byteswap_ulong(size).toString(16)} or 0x${size}.toString(16).`);
         //stream.endianness = ENDIAN_BIG; // TODO: How do we identify the endianness? This doesn't work, at least for the title and desc.
      }
      this.chunkVersion = _byteswap_ushort(stream.readUInt16()); // 08
      this.chunkFlags   = _byteswap_ushort(stream.readUInt16()); // 0A
      this.build = {
         major: stream.readUInt16(), // 0C
         minor: stream.readUInt16(), // 0E
      };
      // Fields after this point are named relative to the chunk body i.e. 
      // when unknown the names are 0x10 too low
      this.contentType = stream.readSInt8(); // gvar_content_type
      stream.skipBytes(3);
      this.fileLength = stream.readUInt32();
      this.unk08 = stream.readUInt64(); // one of these are: owner ID, share ID, server ID
      this.unk10 = stream.readUInt64();
      this.unk18 = stream.readUInt64();
      this.unk20 = stream.readUInt64();
      this.activity = stream.readSInt8(); // gvar_activity
      this.gameMode = stream.readUInt8(); // gvar_game_mode
      this.engine   = stream.readUInt8(); // gvar_game_engine
      stream.skipBytes(1);
      this.unk2C = stream.readUInt32();
      this.engineCategoryIndex = stream.readUInt32();
      this.pad34 = stream.readUInt32();
      this.createdBy  = new VariantContentAuthor();
      this.createdBy.parseBytes(stream);
      this.modifiedBy = new VariantContentAuthor();
      this.modifiedBy.parseBytes(stream);
      stream.assertBytePos(0x0C0);
      this.title       = stream.readWidecharString(128);
      stream.assertBytePos(0x1C0);
      this.description = stream.readWidecharString(128);
      stream.assertBytePos(0x2C0);
      this.engineIconIndex = stream.readUInt32(); // 280
      this.unk284 = stream.readBytes(0x2C);
      stream.assertBytePos(0x2F0);
   }
}