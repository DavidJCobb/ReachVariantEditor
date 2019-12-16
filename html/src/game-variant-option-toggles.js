class GameVariantOptionToggleList {
   constructor(size, stream) {
      this.options = [];
      this.size    = size;
      this.debug_info = {};
      if (!stream)
         this.options = new Array(size);
      else
         this.parse(stream);
   }
   generateDebugInfo() {
      this.debug_info.count_set_bits  = 0;
      this.debug_info.set_bit_indices = [];
      for(let i = 0; i < this.size; i++) {
         if (this.options[i]) {
            this.debug_info.count_set_bits++;
            this.debug_info.set_bit_indices.push(i);
         }
      }
   }
   parse(stream) {
      this.options = [];
      //
      // dword count: (bits + 31) / 32
      //
      let dwordCount = Math.floor((this.size + 31) / 32);
      for(let i = 0; i < dwordCount; i++) {
         let dword = stream.readUInt32({ endianness: ENDIAN_BIG }); // JS bitstream reads things in backwards endianness :(
         //
         // for big-endian dwords (i.e. KSoft.Tool in-memory):
         //    words[bit / 32] & ((1 << 31) >> (bit % 32))
         //
         // for little-endian dwords (i.e. file format):
         //    words[bit / 32] & (1 << (bit % 32))
         //
         for(let j = 0; j < 32; j++) {
            let index = (i * 32) + j;
            if (index >= this.size)
               break;
            this.options[index] = (dword & (1 << j)) != 0;
         }
      }
      this.generateDebugInfo();
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