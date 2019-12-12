class Bitnumber {
   constructor(bits, default_value, options) {
      this.value = default_value || 0;
      Object.defineProperty(this, "bits", {
         writeable: false,
         value:     bits,
      });
      if (!options)
         options = {};
      this.options = {
         enum:   options.enum   || null, // TODO: USE ME // enum
         flags:  options.flags  || null, // TODO: USE ME // flags list
         offset: options.offset || 0, // value is incremented by this much when saved to the file (Reach increments some values so that -1 becomes 0; we have to decrement on load)
         signed: options.signed || false,
         swap:   options.swap   || false, // TODO: support this for BigInts
      };
      if (options.presence_bit !== void 0) {
         //
         // The value is preceded by a single bit. If that bit is 
         // not equal to (options.presence_bit), then the value is 
         // not present. When the value is not present, it will be 
         // set to (if_absent).
         //
         this.options.presence_bit = options.presence_bit;
         this.options.if_absent    = options.if_absent || 0;
      }
      //
      if (bits > 32) {
         if (bits != 64)
            throw new TypeError("Bitnumber doesn't currently support reading large ints that are not exactly 64 bits.");
         this.value          = BigInt(default_value || 0);
         this.options.offset = BigInt(this.options.offset);
         if (options.presence_bit !== void 0)
            this.options.if_absent = BigInt(this.options.if_absent);
      }
   }
   //
   /*bool*/ isInt64() /*const*/ { return this.bits == 64; }
   /*variant*/ castNumber(v) /*const*/ {
      return this.isInt64() ? BigInt(v) : Number(v);
   }
   //
   _findSimpleReadMethod() {
      switch (this.bits) {
         case 8:
         case 16:
         case 32:
         case 64:
            break;
         default:
            return null;
      }
      return ("read" + (this.options.signed ? "S" : "U") + "Int" + this.bits);
   }
   _findSimpleSwapFunction() {
      switch (this.bits) {
         case 32:
            return _byteswap_ulong;
         case 64:
            return _byteswap_uint64;
      }
      return null;
   }
   //
   assign(v) {
      if (this.isInt64()) {
         v = BigInt(v);
         if (v < 0 && !this.options.signed)
            this.value = v & this.max();
         else
            this.value = v;
         return;
      }
      //
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
      return this.assign(this.value - this.castNumber(1));
   }
   increment() {
      // TODO: enum support?
      return this.assign(this.value + this.castNumber(1));
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
      if (this.isInt64()) {
         if (this.options.signed)
            return 0x7FFFFFFFFFFFFFF;
         return 0xFFFFFFFFFFFFFFFF;
      }
      //
      let b = this.bits;
      if (this.options.signed)
         b--;
      let i = 1 << b;
      return i | (i - 1);
   }
   /*Number*/ min() /*const*/ { // returns min possible value
      if (this.isInt64()) {
         if (!this.options.signed)
            return 0;
         return -9223372036854775808n;
      }
      //
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
      {
         let method = this._findSimpleReadMethod();
         if (method && bitstream[method]) {
            this.value = bitstream[method]();
            if (this.options.swap) {
               let swap = this._findSimpleSwapFunction();
               if (swap)
                  this.value = swap(this.value);
            }
         } else {
            this.value = bitstream.readBits(this.bits, {
               swap:   this.options.swap,
               signed: this.options.signed,
            })
         }
      }
      this.value -= this.options.offset;
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

class Bitbool {
   constructor(default_value, options) {
      this.value = default_value || false;
      if (!options)
         options = {};
      this.options = {};
   }
   //
   assign(v) {
      this.value = !!v;
   }
   //
   read(bitstream) {
      this.value = bitstream.readBits(1);
   }
   save(bitstream) {
      throw new Error("Bitbool::save: Not implemented!");
   }
   /*String*/ toString() /*const*/ {
      return this.value.toString.apply(this.value, arguments);
   }
   [Symbol.toPrimitive](hint) /*const*/ {
      if (hint == "string")
         return this.value + "";
      if (hint == "number")
         return +this.value;
      return this.value;
   }
}