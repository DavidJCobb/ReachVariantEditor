class Bitnumber {
   constructor(bits, default_value, options) {
      this.value = default_value || 0;
      this.bits  = bits;
      if (!options)
         options = {};
      this.options = {
         enum:   options.enum   || null, // TODO: USE ME
         offset: options.offset || 0, // value is incremented by this much when saved to the file (Reach increments some values so that -1 becomes 0; we have to decrement on load)
         signed: options.signed || false,
         swap:   options.swap   || false,
      };
      if (options.presence_bit !== void 0) {
         //
         // The value is preceded by a single bit. If that bit is 
         // not equal to (options.presence_bit), then the value is 
         // not present. When the value is not present, it will be 
         // set to (if_absent).
         //
         this.options.presence_bit = options.presence_bit;
         this.options.if_absent    = options.if_absent;
      }
   }
   //
   assign(v) {
      if (v >= 0) {
         this.value = v & this.max();
         return;
      }
      v = v & this.max();
      if (this.options.signed) {
         let m = 1 << (this.bits - 1);
         bytes = (v ^ m) - m;
      }
      this.value = v;
   }
   decrement() {
      // TODO: enum support?
      return this.assign(this.value - 1);
   }
   increment() {
      // TODO: enum support?
      return this.assign(this.value + 1);
   }
   //
   friendly() /*const*/ {
      if (this.options.enum) {
         //
         // TODO: Convert from the raw value to the friendly one, e.g. 
         // Movement Speed 100% is (6) internally; given this.value == 6, 
         // this would return 100.
         //
         throw new Error("Bitnumber::friendly: Not implemented!");
      }
      return this.value;
   }
   /*Number*/ max() /*const*/ { // returns max possible value
      let b = this.bits;
      if (this.options.signed)
         b--;
      let i = 1 << b;
      return i | (i - 1);
   }
   /*Number*/ min() /*const*/ { // returns min possible value
      if (!this.options.signed)
         return 0;
      let b = this.bits - 1;
      return -(1 << b);
   }
   read(bitstream) {
      if (this.options.presence_bit !== void 0) {
         let bit = bitstream.readBits(1);
         if (bit != this.options.presence_bit) {
            this.value = this.options.if_absent;
            return;
         }
      }
      this.value = bitstream.readBits(this.bits, {
         swap:   this.options.swap,
         signed: this.options.signed,
      }) - this.options.offset;
   }
   save(bitstream) {
      throw new Error("Bitnumber::save: Not implemented!");
      // remember to save (this.value + this.options.offset), not (this.value)
   }
   /*String*/ toString() /*const*/ {
      return this.value.toString.apply(this.value, arguments);
   }
   [Symbol.toPrimitive](hint) /*const*/ {
      if (hint == "string")
         return this.value + "";
      return this.value;
   }
}