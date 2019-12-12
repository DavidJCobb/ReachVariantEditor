const MEGALO_OPTION_VALUE_VALUE_BITCOUNT = 10;

class GameVariantRawMegaloOptionValue {
   constructor(stream, option) {
      this.value = stream.readBits(MEGALO_OPTION_VALUE_VALUE_BITCOUNT); // TODO: sign-extend
      if (!option.isRangeValue) {
         this.nameStringIndex = stream.readMegaloVariantStringIndex();
         this.descStringIndex = stream.readMegaloVariantStringIndex();
      }
   }
}

const MEGALO_OPTION_VALUE_COUNT_BITCOUNT = _bitcount(8);
const MEGALO_OPTION_VALUE_INDEX_BITCOUNT = _bitcount(8 - 1);
class GameVariantRawMegaloOption {
   constructor(stream) {
      this.nameStringIndex = stream.readMegaloVariantStringIndex();
      this.descStringIndex = stream.readMegaloVariantStringIndex();
      this.isRangeValue    = stream.readBits(1) != 0;
      this.values = [];
      if (this.isRangeValue) {
         this.rangeDefaultValue = stream.readBits(MEGALO_OPTION_VALUE_VALUE_BITCOUNT); // TODO: sign-extend
         this.rangeMinValue = new GameVariantRawMegaloOptionValue(stream, this);
         this.rangeMaxValue = new GameVariantRawMegaloOptionValue(stream, this);
         //
         this.defaultValueIndex = null;
      } else {
         this.rangeDefaultValue = null;
         this.rangeMinValue = null;
         this.rangeMaxValue = null;
         //
         this.defaultValueIndex = stream.readBits(MEGALO_OPTION_VALUE_INDEX_BITCOUNT, BIT_ORDER_UNKNOWN);
         let valueCount = stream.readBits(MEGALO_OPTION_VALUE_COUNT_BITCOUNT, BIT_ORDER_UNKNOWN);
         for(let i = 0; i < valueCount; i++)
            this.values[i] = new GameVariantRawMegaloOptionValue(stream, this);
         //
         if (this.defaultValueIndex >= valueCount)
            console.warn("Megalo option with out-of-bounds default value index.");
      }
      //
      if (this.isRangeValue) {
         this.currentValue = stream.readBits(MEGALO_OPTION_VALUE_VALUE_BITCOUNT); // TODO: sign-extend
      } else {
         this.currentValueIndex = stream.readBits(MEGALO_OPTION_VALUE_INDEX_BITCOUNT, BIT_ORDER_UNKNOWN);
      }
   }
}