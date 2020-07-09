class UGCHeader {
   constructor(stream) {
      this.type       = stream.readBits(4, BIT_ORDER_UNKNOWN) - 1; // ReachFileType
         // ^ "None" is encoded as 0 instead of -1 in mpvr; the whole enum is shifted up by 1
         //   when written to the file.
      // Note: As of the previous field, we are byte-aligned again.
      this.fileLength = _byteswap_ulong(stream.readUInt32());
      stream.reportOffset();
      this.unk08      = _byteswap_uint64(stream.readUInt64());
      this.unk10      = _byteswap_uint64(stream.readUInt64());
      this.unk18      = _byteswap_uint64(stream.readUInt64());
      this.unk20      = _byteswap_uint64(stream.readUInt64());
      stream.reportOffset();
      this.activity   = stream.readBits(3, false) - 1;
         // ^ "None" is encoded as 0 instead of -1 in mpvr; the whole enum is shifted up by 1
         //   when written to the file.
      this.gameMode   = stream.readBits(3, false);
      this.engine     = stream.readBits(3, false);
      this.mapID      = stream.readBits(32, false); // the ID of the map we're built on
      this.engineCategoryIndex = stream.readBits(8, false); // TODO: we need to sign-extend it from one byte to an SInt32
      this.createdBy   = new VariantContentAuthor();
      this.createdBy.parseBits(stream);
      this.modifiedBy  = new VariantContentAuthor();
      this.modifiedBy.parseBits(stream);
      let endianness = stream.endianness;
      stream.endianness = ENDIAN_BIG; // KSoft.Tool always uses big-endian, but MCC may be little-endian for this?
      this.title       = stream.readWidecharString(128, true);
      this.description = stream.readWidecharString(128, true);
      switch (this.type) {
         case 3: // film
         case 4: // film clip
            this.unk280 = stream.readSInt32();
            break;
         case 6: // game variant
            this.unk280 = stream.readBits(8, BIT_ORDER_UNKNOWN); // engineIconIndex
            break;
      }
      if (this.activity == 2)
         this.hopperID = stream.readUInt16(); // TODO: TEST ME (how?)
      else
         this.hopperID = null;
      if (this.gameMode == 1) {
         this.unk02A0 = stream.readBits(8, false);
         this.unk02A1 = stream.readBits(2, false);
         this.unk02A2 = stream.readBits(2, false);
         this.unk02A3 = stream.readBits(8, false);
         this.unk02A4 = stream.readBits(32, false);
      } else if (this.gameMode == 2) {
         this.unk02A0 = stream.readBits(2, false);
         this.unk02A4 = stream.readBits(32, false);
      }
   }
}