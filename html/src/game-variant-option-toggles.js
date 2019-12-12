class GameVariantOptionToggleList {
   constructor(size, stream) {
      this.options = [];
      this.size    = size;
      if (!stream)
         this.options = new Array(size);
      else
         this.parse(stream);
   }
   parse(stream) {
      this.options = [];
      //
      // word count: (bits + 15) / 16
      //
      let wordCount = Math.floor((this.size + 15) / 16);
      for(let i = 0; i < wordCount; i++) {
         let word = stream.readUInt16({ endianness: ENDIAN_LITTLE });
         //
         // for big-endian words (i.e. KSoft.Tool in-memory):
         //    words[bit / 16] & ((1 << 15) >> (bit % 16))
         //
         // for little-endian words (i.e. file format):
         //    words[bit / 16] & (1 << (bit % 16))
         //
         for(let j = 0; j < 16; j++) {
            let index = (i * 16) + j;
            if (index >= this.size)
               break;
            this.options[index] = (word & (1 << j)) != 0;
         }
      }
   }
}

class GameVariantEngineOptionToggleList extends GameVariantOptionToggleList {
   constructor(stream) {
      super(1272, stream);
   }
}
class GameVariantMegaloOptionToggleList extends GameVariantOptionToggleList {
   constructor(stream) {
      super(16, stream);
   }
}