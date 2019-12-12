let TU1Flags = new TypedBitfield({
   name: "TU1Flags",
}, {
   bleedThrough: 0x01,
   disableArmorLockStickyShed: 0x02, // armor lock can't shed stickies when activated
   armorLockCanBeStuck:        0x04, // armor lock can be stuck after it's been turned on
   enableActiveCamoModifiers:  0x08, // enable TU1 camo modifiers
   onlySwordCanBlockSword:     0x10,
   enableFullAutoMagnum:       0x20,
   unused40: 0x40,
   unused80: 0x80,
});
class GameVariantTU1Data {
   //
   // NOTES:
   //
   // These are absolute, not relative to the pre-TU values. To wit, a precision 
   // bloom of 0.7677165354330708 is the vanilla value, while a precision bloom 
   // of 0.6496062992125984 is the TU value (i.e. 0.64 / 0.76 == 0.85 or 85% 
   // bloom, which is to spec).
   //
   constructor(stream) { // sizeof == 0xB
      if (stream)
         this.parse(stream);
      else {
         //
         // Vanilla settings:
         //
         this.flags = TU1Flags.make(0);
         this.precisionBloom            = 0.7677165354330708;
         this.activeCamoEnergy          = 0.04330708661417323;
         this.activeCamoEnergyBonus     = 0;
         this.armorLockDamageDrain      = 1.5551181102362204;
         this.armorLockDamageDrainLimit = 0;
         this.magnumDamage              = 2.8149606299212597;
         this.magnumFireRate            = 0.6102362204724409;
         this.precisionBloom            = 0.7677165354330708;
      }
   }
   parse(stream) {
      this.flags = TU1Flags.make(stream.readUInt32()); // 00
      this.precisionBloom            = stream.readCompressedFloat(8, 0.0, 10.0, false, true); // 04
      this.armorLockDamageDrain      = stream.readCompressedFloat(8, 0.0,  2.0, false, true); // 05
      this.armorLockDamageDrainLimit = stream.readCompressedFloat(8, 0.0,  2.0, false, true); // 06
      this.activeCamoEnergyBonus     = stream.readCompressedFloat(8, 0.0,  2.0, false, true); // 07
      this.activeCamoEnergy          = stream.readCompressedFloat(8, 0.0,  2.0, false, true); // 08
      this.magnumDamage              = stream.readCompressedFloat(8, 0.0, 10.0, false, true); // 09
      this.magnumFireRate            = stream.readCompressedFloat(8, 0.0, 10.0, false, true); // 0A
   }
}