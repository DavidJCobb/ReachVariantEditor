const chunk_flags = Object.freeze({
   initialized: 1, // always set
   is_header:   2, // _blf, chdr
});

class GameVariantBlamHeader {
   constructor(stream) {
      this.signature = stream.readString(4);
      if (this.signature != "_blf")
         throw new Error(`GameVariantBlamHeader expected signature "_blf"; got "${this.signature}".`);
      let size = stream.readUInt32();
      if (size != 0x30) {
         size = _byteswap_ulong(size);
         if (size != 0x30)
            throw new Error(`GameVariantBlamHeader expected size 0x30; got 0x${_byteswap_ulong(size).toString(16)} or 0x${size}.toString(16).`);
         stream.endianness = ENDIAN_BIG;
      }
      this.chunkVersion = _byteswap_ushort(stream.readUInt16()); // 08
      this.chunkFlags   = _byteswap_ushort(stream.readUInt16()); // 0A
      this.unk0C = stream.readUInt16();
      this.unk0E = stream.readBytes(0x20);
      this.unk2E = stream.readBytes(0x02);
      stream.assertBytePos(0x30);
   }
}