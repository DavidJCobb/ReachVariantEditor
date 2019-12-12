let LoadoutOptionFlags = new TypedBitfield({
   name: "LoadoutOptionFlags",
}, {
   mapLoadoutsEnabled: 0x01, // verify
   unknown02:          0x02,
});

class GameVariantLoadout {
   constructor(stream) {
      this.flags = stream.readBits(0); // "enabled" flag
      this.nameIndex       = -1;
      this.weaponPrimary   = -3;
      this.weaponSecondary = -3;
      this.ability         = -3;
      this.grenadeCount    =  0;
      //
      if (stream)
         this.parse(stream);
   }
   parse(stream) {
      this.flags = stream.readBits(1); // "enabled" flag
      {
         let absent = stream.readBits(1) != 0;
         if (!absent)
            this.nameIndex = stream.readBits(7, BIT_ORDER_UNKNOWN);
         else
            this.nameIndex = -1;
      }
      this.weaponPrimary   = stream.readSInt8();
      this.weaponSecondary = stream.readSInt8();
      this.ability         = stream.readSInt8();
      this.grenadeCount    = stream.readBits(4, BIT_ORDER_UNKNOWN);
   }
}
class GameVariantLoadoutPalette {
   constructor(stream) {
      this.loadouts = [];
      for(let i = 0; i < 5; i++)
         this.loadouts[i] = new GameVariantLoadout(stream);
   }
}