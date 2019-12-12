let SingleTeamFlags = new TypedBitfield({
   name: "SingleTeamFlags",
}, {
   enabled:                0x01, // this team is used in this gametype
   overrideColorPrimary:   0x02,
   overrideColorSecondary: 0x04,
   overrideColorText:      0x08,
});

class GameVariantTeam {
   constructor(stream) {
      this.flags = SingleTeamFlags.make(0);
      {
         this.names = new LocalizedStringTable(1, 0x20);
         this.names.offsetBitlength     = 5;
         this.names.bufferSizeBitlength = 6;
      }
      this.initialDesignator = 0;
      this.spartanOrElite    = 0;
      this.colorPrimary   = -1;
      this.colorSecondary = -1;
      this.colorText      = -1;
      this.fireteamCount = 1;
      //
      if (stream)
         this.parse(stream);
   }
   parse(stream) {
      this.flags = SingleTeamFlags.make(stream.readBits(4, BIT_ORDER_UNKNOWN));
      {
         this.names = new LocalizedStringTable(1, 0x20);
         this.names.offsetBitlength     = 5;
         this.names.bufferSizeBitlength = 6;
         //
         this.names.parse(stream);
      }
      this.initialDesignator = stream.readBits(4, BIT_ORDER_UNKNOWN) - 1;
      this.spartanOrElite = stream.readBits(1); // 0 = Spartan, 1 = Elite; only used if gametype player species is "team default?"
      this.colorPrimary   = stream.readSInt32();
      this.colorSecondary = stream.readSInt32();
      this.colorText      = stream.readSInt32();
      this.fireteamCount  = stream.readBits(5, BIT_ORDER_UNKNOWN);
   }
}