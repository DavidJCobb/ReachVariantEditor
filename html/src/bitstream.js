const ENDIAN_LITTLE = Symbol("little-endian"); // 0x11223344 -> 44 33 22 11
const ENDIAN_BIG    = Symbol("big-endian");    // 0x11223344 -> 11 22 33 44

const DEBUG_BITSTREAM_READ_BIT_ORDER = false;

const BIT_ORDER_UNKNOWN = void 0;

class Bitstream {
   constructor(buf) {
      this.buffer = buf;
      this.view   = new DataView(buf);
      this.offset = 0; // in bits
      this.endianness = ENDIAN_LITTLE;
      //
      this.log_state = {
         start: null,
      };
   }
   /*int*/ getBitPos() {
      return this.offset;
   }
   /*int*/ getBitOffset() {
      return this.offset % 8;
   }
   /*int*/ getBytePos() {
      return Math.floor(this.offset / 8);
   }
   prepToReportBitnumber() {
      console.assert(this.log_state.start === null);
      this.log_state.start = this.offset;
   }
   reportBitnumber(t) {
      console.assert(this.log_state.start !== null);
      let start = this.log_state.start;
      this.log_state.start = null;
      console.log(`${t ? t + " - " : ""}Bitnumber at offset 0x${Math.floor(start / 8).toString(16)} bytes + ${start % 8} bits, length ${this.offset - start} bits`);
   }
   reportOffset(t) {
      console.log(`${t ? t + " - " : ""}Bitstream is currently at 0x${this.getBytePos().toString(16)}+${this.getBitOffset()}b.`);
   }
   backToStart() {
      this.offset = 0;
   }
   skipBits(count) {
      if (count <= 0)
         return;
      this.offset += count;
   }
   skipBytes(count) {
      if (count <= 0)
         return;
      this.offset += count * 8;
   }
   skipToBytePos(pos) {
      if (this.offset > pos * 8)
         throw new Error("We have passed that position.");
      this.offset = pos * 8;
   }
   assertBytePos(pos) {
      let off = this.offset / 8;
      if (off != pos)
         throw new Error(`Assertion failed: we should be at stream position 0x${pos.toString(16)} but are at 0x${off.toString(16)}.`);
   }
   //
   _endiannessBoolFromOptions(options) {
      if (!options || !options.endianness)
         return this.endianness == ENDIAN_LITTLE;
      return options.endianness == ENDIAN_LITTLE;
   }
   //
   /*int*/ readBits(count, options) {
      if (count <= 0)
         return 0;
      if (options === !!options)
         options = { swap: options };
      else if (!options)
         options = {};
      let pos   = Math.floor(this.offset / 8);
      let shift = this.offset % 8;
      if (DEBUG_BITSTREAM_READ_BIT_ORDER && options.swap === void 0)
         console.warn(`WARNING: Bit order unspecified for read of ${count} bits from 0x${pos}+${shift}b.`);
      let bytes = this.view.getUint8(pos) & (0xFF >>> shift);
      //
      let bits_read = 8 - shift;
      if (count < bits_read) {
         bytes = bytes >>> (bits_read - count);
         if (options.swap && !options.recursing)
            bytes = _bitswap(bytes, count);
         this.offset += count;
         return bytes;
      }
      this.offset += bits_read;
      let remaining = count - bits_read;
      if (remaining) {
         let next = this.readBits(remaining, { recursing: true });
         bytes = (bytes << remaining) | next;
      }
      if (options.swap) // NOTE: if KSoft says to swap it, then do the opposite?
         bytes = _bitswap(bytes, count);
      if (options.signed) {
         let m = 1 << (count - 1);
         bytes = (bytes ^ m) - m;
      }
      if (this.offset != (pos * 8 + shift) + count)
         throw new Error("Assertion failed: didn't advance the offset properly.");
      return bytes;
   }
   /*int*/ readByte() {
      return this.readBits(8);
   }
   /*array<int>*/ readBytes(count) {
      let arr = [];
      for(let i = 0; i < count; i++)
         arr.push(this.readByte());
      return arr;
   }
   /*DataView*/ _getByteView(count) {
      let bytes  = this.readBytes(count);
      let buffer = new ArrayBuffer(count);
      let view   = new DataView(buffer);
      for(let i = 0; i < count; i++)
         view.setUint8(i, bytes[i]);
      return view;
   }
   /*DataView*/ _getDword() {
      return this._getByteView(4);
   }
   /*bool*/ readBool() {
      return this.readByte() != 0;
   }
   /*int*/ readUInt8() {
      return this.readByte();
   }
   /*int*/ readSInt8() {
      return this._getByteView(1).getInt8(0);
   }
   /*int*/ readUInt16(options) {
      return this._getByteView(2).getUint16(0, this._endiannessBoolFromOptions(options));
   }
   /*int*/ readSInt16(options) {
      return this._getByteView(2).getInt16(0, this._endiannessBoolFromOptions(options));
   }
   /*int*/ readUInt32(options) {
      return this._getDword().getUint32(0, this._endiannessBoolFromOptions(options));
   }
   /*int*/ readSInt32(options) {
      return this._getDword().getInt32(0, this._endiannessBoolFromOptions(options));
   }
   /*float*/ readFloat(options) {
      return this._getDword().getFloat32(0, this._endiannessBoolFromOptions(options));
   }
   /*BigInt*/ readUInt64(options) {
      return this._getByteView(8).getBigUint64(0, this._endiannessBoolFromOptions(options));
   }
   /*BigInt*/ readSInt64(options) {
      return this._getByteView(8).getBigInt64(0, this._endiannessBoolFromOptions(options));
   }
   /*string*/ readString(length, stopEarlyOnNull) {
      let s = "";
      let i = 0;
      for(; i < length; i++) {
         let c = this.readByte();
         if (!c)
            break;
         s += String.fromCharCode(c);
      }
      if (!stopEarlyOnNull)
         this.skipBytes(length - i - 1);
      return s;
   }
   /*string*/ readWidecharString(length, stopEarlyOnNull) {
      let s = "";
      let i = 0;
      for(; i < length; i++) {
         let wc = this.readUInt16();
         if (!wc)
            break;
         s += String.fromCharCode(wc);
      }
      if (!stopEarlyOnNull)
         this.skipBytes((length - i - 1) * 2);
      return s;
   }
   readMegaloVariantStringIndex() {
      return this.readBits(_bitcount(112 - 1), false);
   }
   readCompressedFloat(bitcount, min, max, signed, unknown) {
      //
      // Presumably this was reverse-engineered from the game. KSoft.Tool's 
      // source code doesn't explain it, but does link to a Stack Overflow 
      // answer on converting a 32-bit float to a 16-bit float, with the 
      // caption "interesting." <http://stackoverflow.com/a/3542975/444977>
      //
      let raw   = this.readBits(bitcount);
      let range = max - min;
      let result;
      //
      let precision = 1 << bitcount;
      if (signed)
         precision -= 1;
      // given an 8-bit float:
      //  * precision == 256 if unsigned i.e. there are 256 possible values including 0
      //  * precision == 255 if signed   i.e. there are 255 possible values including 0
      if (unknown) {
         if (!raw)
            result = min;
         else if (raw == precision - 1) // least significant bit is the sign bit?
            result = max;
         else {
            result = min + (raw - 0.5) * (range / (precision - 2));
            //
            // from math in KSoft.Tool (presumably reverse-engineered):
            //
            // result == min + (r6 * (range / r8)) + (.5 * (range / r8))
            // result == min + ((raw - 1) * (range / (precision - 2))) + (.5 * (range / (precision - 2)))
            // result == min + (raw - 0.5) * (range / (precision - 2))
            //
            // (result - min) / (range / (precision - 2)) + 0.5 = raw
         }
      } else {
         result = min + (raw + 0.5) * (range / precision);
         //
         // from math in KSoft.Tool (presumably reverse-engineered):
         //
         // result == min + (raw * range / precision) + (0.5 * range / precision)
         // result == min + (raw + 0.5) * (range / precision)
         //
         // (result - min) / (range / precision) - 0.5 == raw
      }
      if (signed) {
         precision--;
         if (raw * 2 == precision) // if raw == 127, for an 8-bit float
            result = (min + max) * 0.5;
      }
      return result;
   }
}