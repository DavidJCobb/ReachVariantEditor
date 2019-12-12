class VariantContentAuthor {
   constructor() {
      this.date       = new Date(0);
      this.xuid       = BigInt(0);
      this.author     = "";
      this.isOnlineID = false;
   }
   parseBytes(stream) {
      {
         let timestamp = stream.readUInt64();
         this.date = new Date(0); // Unix epoch: Jan 1 1970 midnight GMT
         this.date.setSeconds(Number(timestamp));
      }
      this.xuid       = stream.readUInt64();
      this.author     = stream.readString(16, false); // ASCII CString, always 16-bytes in CHDR and variable-length capped to 16 bytes in MPVR; length includes null terminator
      this.isOnlineID = stream.readBool();
      stream.skipBytes(3);
   }
   parseBits(stream) {
      {
         let timestamp = stream.readUInt64();
         timestamp = _byteswap_uint64(timestamp);
         this.date = new Date(0); // Unix epoch: Jan 1 1970 midnight GMT
         this.date.setSeconds(Number(timestamp));
      }
      this.xuid       = stream.readUInt64();
      this.author     = stream.readString(16, true); // ASCII CString, always 16-bytes in CHDR and variable-length capped to 16 bytes in MPVR; length includes null terminator
      this.isOnlineID = stream.readBits(1) != 0;
   }
}